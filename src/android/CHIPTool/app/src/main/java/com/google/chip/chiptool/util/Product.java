package com.google.chip.chiptool.util;

import java.util.Arrays;
import java.util.Map;
import java.util.function.Function;
import java.util.stream.Collectors;

public enum Product {
    FIRE_TV_STICK("1", "Fire TV Stick"),
    FIRE_TV_CUBE("456", "Fire TV Cube"),
    LG_OLED_GX("3", "LG OLED GX Series"),
    LG_NANOCELL_90("4", "LG NanoCell 90 Series");

    private final String id;
    private final String name;

    static final Map<String, Product> names = Arrays.stream(Product.values())
            .collect(Collectors.toMap(Product::getName, Function.identity()));
    static final Map<String, Product> ids = Arrays.stream(Product.values())
            .collect(Collectors.toMap(Product::getId, Function.identity()));

    Product(String id, String name) {
        this.id = id;
        this.name = name;
    }

    public String getId() {
        return id;
    }

    public String getName() {
        return name;
    }

    public static Product fromName(final String name) {
        return names.get(name);
    }

    public static Product fromId(final String id) {
        return ids.get(id);
    }

    @Override
    public String toString() {
        return name;
    }
}
