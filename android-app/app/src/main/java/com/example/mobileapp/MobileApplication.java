package com.example.mobileapp;

import android.app.Application;
import com.example.mobileapp.services.ApiClient;

public class MobileApplication extends Application {
    @Override
    public void onCreate() {
        super.onCreate();
        ApiClient.initialize(this);
    }
}
