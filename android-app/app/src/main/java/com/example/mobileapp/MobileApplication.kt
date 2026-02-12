package com.example.mobileapp

import android.app.Application
import com.example.mobileapp.services.ApiClient

class MobileApplication : Application() {
    override fun onCreate() {
        super.onCreate()
        ApiClient.initialize(this)
    }
}
