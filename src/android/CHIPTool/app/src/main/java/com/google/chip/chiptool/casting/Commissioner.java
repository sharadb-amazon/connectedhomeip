package com.google.chip.chiptool.casting;

import com.google.chip.chiptool.util.DeviceType;
import com.google.chip.chiptool.util.Product;
import com.google.chip.chiptool.util.Vendor;

import java.net.InetAddress;

public class Commissioner {
    private String serviceName;
    private String serviceType;
    private InetAddress host;
    private int port;
    private Vendor vendor;
    private DeviceType deviceType;
    private Product product;
    private String deviceName;

    public String getServiceName() {
        return serviceName;
    }

    public void setServiceName(String serviceName) {
        this.serviceName = serviceName;
    }

    public String getServiceType() {
        return serviceType;
    }

    public void setServiceType(String serviceType) {
        this.serviceType = serviceType;
    }

    public InetAddress getHost() {
        return host;
    }

    public void setHost(InetAddress host) {
        this.host = host;
    }

    public int getPort() {
        return port;
    }

    public void setPort(int port) {
        this.port = port;
    }

    public Vendor getVendor() {
        return vendor;
    }

    public void setVendor(Vendor vendor) {
        this.vendor = vendor;
    }

    public DeviceType getDeviceType() {
        return deviceType;
    }

    public void setDeviceType(DeviceType deviceType) {
        this.deviceType = deviceType;
    }

    public Product getProduct() {
        return product;
    }

    public void setProduct(Product product) {
        this.product = product;
    }

    public String getDeviceName() {
        return deviceName;
    }

    public void setDeviceName(String deviceName) {
        this.deviceName = deviceName;
    }

    @Override
    public String toString() {
        return "Commissioner{" +
                "serviceName='" + serviceName + '\'' +
                ", serviceType='" + serviceType + '\'' +
                ", host=" + host +
                ", port=" + port +
                ", vendor=" + vendor +
                ", deviceType=" + deviceType +
                ", product=" + product +
                ", deviceName='" + deviceName + '\'' +
                '}';
    }
}
