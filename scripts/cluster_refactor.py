import re
import sys
import traceback
import os

# List of Cluster classes to skip
block_list = {"SampleMeiCluster"}

def preserve_camel_case(name):
    return name[0].upper() + name[1:]

def extract_command_args_code(function_content):
    start_marker = "ArrayList<StructElement> elements = new ArrayList<>();"
    end_marker = "StructType commandArgs = new StructType(elements);"
    
    try:
        start_idx = function_content.index(start_marker)
        end_idx = function_content.index(end_marker) + len(end_marker)
        return function_content[start_idx:end_idx]
    except ValueError:
        print(f"  [WARN] Command arguments block not found in function: {function_content[:100]}...")
        return None

def find_functions_with_args(class_content):
    functions = []
    current_pos = 0
    while True:
        match = re.search(r'public void (\w+)\((.*?)\) {', class_content[current_pos:], re.DOTALL)
        if not match:
            break

        start_pos = current_pos + match.start()
        function_name = match.group(1)
        full_params = match.group(2)

        brace_count = 1
        pos = current_pos + match.end()

        while brace_count > 0 and pos < len(class_content):
            if class_content[pos] == '{':
                brace_count += 1
            elif class_content[pos] == '}':
                brace_count -= 1
            pos += 1

        if brace_count == 0:
            function_content = class_content[start_pos:pos]
            if "ArrayList<StructElement> elements = new ArrayList<>();" in function_content:
                functions.append((function_name, function_content, full_params))
            else:
                print(f"  [INFO] Skipping {function_name}: No command argument block detected.")

        current_pos = pos

    return functions

def process_cluster_class(class_content, cluster_name):
    print(f"[PROCESS] Processing {cluster_name}Cluster class...")
    
    if f"public {cluster_name}Cluster() {{}}" not in class_content:
        class_content = re.sub(r'\{', f'{{\n    public {cluster_name}Cluster() {{}}', class_content, count=1)
        print(f"  [ADD] Added no-args constructor to {cluster_name}Cluster.")
    
    modifications = []
    new_methods = []
    functions = find_functions_with_args(class_content)

    for function_name, function_content, full_params in functions:
        command_args_code = extract_command_args_code(function_content)
        if command_args_code:
            print(f"  [FOUND] Command args in function: {function_name}")
            param_list = [p.strip() for p in full_params.split(',') if not p.strip().endswith("Callback callback")]
            param_names = [p.split()[-1] for p in param_list]
            param_string = ", ".join(param_list)
            new_method_name = f"construct{preserve_camel_case(function_name)}Args"
            new_method = f"""
    public StructType {new_method_name}({param_string}) {{
        {command_args_code}
        return commandArgs;
    }}"""
            modified_function = function_content.replace(
                command_args_code,
                f"StructType commandArgs = {new_method_name}({', '.join(param_names)});"
            )
            print(f"  [MODIFY] Created new method: {new_method_name} (excluding callback parameter)")
            modifications.append((function_content, modified_function))
            new_methods.append(new_method)

    modified_class_content = class_content
    for old, new in modifications:
        modified_class_content = modified_class_content.replace(old, new)

    if new_methods:
        modified_class_content = modified_class_content.rstrip('}') + '\n' + '\n'.join(new_methods) + "\n}"

    return modified_class_content

def process_file():
    file_path = os.path.join(os.path.dirname(__file__), "../src/controller/java/generated/java/chip/devicecontroller/ChipClusters.java")
    
    try:
        with open(file_path, 'r', encoding='utf-8') as file:
            content = file.read()

        print("[INFO] File successfully read.")

        content = re.sub(
            r'private static native byte\[\] encodeToTlv\(BaseTLVType value\);',
            r'public static native byte[] encodeToTlv(BaseTLVType value);',
            content
        )
        print("[MODIFY] Changed encodeToTlv method visibility to public.")

        content = re.sub(
            r'(class BaseChipCluster \{)',
            r'\1\n    public BaseChipCluster() {}',
            content,
            count=1
        )
        print("[MODIFY] Added no-args constructor to BaseChipCluster.")

        cluster_pattern = r'public static class (\w+)Cluster extends BaseChipCluster \{.*?(?=\n\s+public static class|\Z)'
        clusters = re.finditer(cluster_pattern, content, re.DOTALL)

        modified_content = content
        found_clusters = 0

        for cluster_match in clusters:
            cluster_name = cluster_match.group(1)
            found_clusters += 1

            if f"{cluster_name}Cluster" in block_list:
                print(f"[SKIP] Found {cluster_name}Cluster, skipping (in block list).")
                continue

            print(f"[INFO] Found cluster class: {cluster_name}Cluster")
            cluster_content = cluster_match.group(0)
            modified_cluster_content = process_cluster_class(cluster_content, cluster_name)
            modified_content = modified_content.replace(cluster_content, modified_cluster_content)

        if found_clusters == 0:
            print("[WARN] No cluster classes found in the file.")

        with open(file_path, 'w', encoding='utf-8') as file:
            file.write(modified_content)

        print(f"\n[SUCCESS] Modified file in place: {file_path}")

    except FileNotFoundError:
        print(f"[ERROR] File '{file_path}' not found.")
        sys.exit(1)
    except Exception as e:
        print("[ERROR] An error occurred while processing the file.")
        print(traceback.format_exc())
        sys.exit(1)

if __name__ == "__main__":
    print("\n[START] Processing ChipClusters.java in predefined path\n")
    process_file()

