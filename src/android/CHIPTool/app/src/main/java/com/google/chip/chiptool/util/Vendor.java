package com.google.chip.chiptool.util;

import java.util.Arrays;
import java.util.Map;
import java.util.function.Function;
import java.util.stream.Collectors;

public enum Vendor {
    Amazon("123", "Amazon"),
    LG("2", "LG");

    private final String id;
    private final String name;

    static final Map<String, Vendor> names = Arrays.stream(Vendor.values())
            .collect(Collectors.toMap(Vendor::getName, Function.identity()));
    static final Map<String, Vendor> ids = Arrays.stream(Vendor.values())
            .collect(Collectors.toMap(Vendor::getId, Function.identity()));

    Vendor(String id, String name) {
        this.id = id;
        this.name = name;
    }

    public String getId() {
        return id;
    }

    public String getName() {
        return name;
    }

    public static Vendor fromName(final String name) {
        return names.get(name);
    }

    public static Vendor fromId(final String id) {
        return ids.get(id);
    }

    @Override
    public String toString() {
        return name;
    }
}
