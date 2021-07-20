package com.google.chip.chiptool.util;

import java.util.Arrays;
import java.util.Map;
import java.util.function.Function;
import java.util.stream.Collectors;

public enum DeviceType {
    TV("", "TV"),
    STREAMING_DEVICE("35", "Streaming device"),
    ANDROID_APP("81", "Android app");

    private final String id;
    private final String name;

    static final Map<String, DeviceType> names = Arrays.stream(DeviceType.values())
            .collect(Collectors.toMap(DeviceType::getName, Function.identity()));
    static final Map<String, DeviceType> ids = Arrays.stream(DeviceType.values())
            .collect(Collectors.toMap(DeviceType::getId, Function.identity()));

    DeviceType(String id, String name) {
        this.id = id;
        this.name = name;
    }

    public String getId() {
        return id;
    }

    public String getName() {
        return name;
    }

    public static DeviceType fromName(final String name) {
        return names.get(name);
    }

    public static DeviceType fromId(final String id) {
        return ids.get(id);
    }

    @Override
    public String toString() {
        return name;
    }
}
