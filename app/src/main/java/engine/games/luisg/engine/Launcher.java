package engine.games.luisg.engine;

import android.app.ActionBar;
import android.app.NativeActivity;

import android.content.Context;
import android.content.Intent;
import android.os.Bundle;
import android.os.Handler;
import android.util.Log;
import android.view.Gravity;
import android.view.View;
import android.view.ViewGroup;
import android.view.WindowManager;
import android.widget.ActionMenuView;
import android.widget.LinearLayout;
import android.widget.PopupWindow;

import com.google.android.gms.ads.AdRequest;
import com.google.android.gms.ads.AdView;
import com.google.android.gms.ads.AdSize;
//import com.google.android.gms.ads.identifier.AdvertisingIdClient;

/**
 * Created by blubs on 7/19/2016.
 */

public class Launcher extends NativeActivity
{
	public static Launcher singletonInstance;

	AdView adView;
	LinearLayout layout;
	LinearLayout mainLayout;
	PopupWindow popup;

	boolean adsInitialized = false;

	@Override
	public void onCreate(Bundle savedInstanceState)
	{
		super.onCreate(savedInstanceState);

		Log.println(Log.INFO, "tag", "about to set instance for launcher");
		singletonInstance = this;

		getWindow().addFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);

		adView = new AdView(this);
		//This is for a test ad
		adView.setAdSize(AdSize.BANNER);
		adView.setAdUnitId("ca-app-pub-3940256099942544/6300978111");
		//This is for using my actual ad unit id
		//view.setAdUnitId(DevData.adUnitId);

		//The following block of code is for test initializing an ad 5 seconds after startup

		//Deferring the following code due to delayed startup
		final Handler handler = new Handler();
		handler.postDelayed(new Runnable()
		{
			@Override
			public void run()
			{
				setupAdView();
			}
		}, 3000);
	}

	public void setupAdView()
	{
		if(adsInitialized)
			return;
		if(adView != null)
		{
			singletonInstance.runOnUiThread(new Runnable()
			{
				@Override
				public void run()
				{
					adsInitialized = true;
					popup = new PopupWindow(singletonInstance);
					//Minimum size for admob
					popup.setWidth(320);
					popup.setHeight(50);

					popup.setWidth(LinearLayout.LayoutParams.WRAP_CONTENT);
					popup.setHeight(LinearLayout.LayoutParams.WRAP_CONTENT);
					popup.setClippingEnabled(false);
					layout = new LinearLayout(singletonInstance);
					mainLayout = new LinearLayout(singletonInstance);

					layout.setPadding(-5, -5, -5, -5);

					ViewGroup.MarginLayoutParams params = new ViewGroup.MarginLayoutParams(ViewGroup.LayoutParams.WRAP_CONTENT,ViewGroup.LayoutParams.WRAP_CONTENT);
					params.setMargins(0, 0, 0, 0);
					layout.setOrientation(LinearLayout.VERTICAL);
					layout.addView(adView, params);

					popup.setContentView(layout);
					singletonInstance.setContentView(mainLayout,params);

					Log.println(Log.INFO, "jni", "============= Initiating Ad Startup ==============");
					AdRequest.Builder adRequestBuilder = new AdRequest.Builder();

					//For testing on emulator
					//adRequestBuilder.addTestDevice(AdRequest.DEVICE_ID_EMULATOR);
					//Supposedly for test ads
					//adRequestBuilder.addTestDevice("BANANANAANANANANANANNANANANANANA");
					//Log recommends using this device id to get test ads
					adRequestBuilder.addTestDevice("3218CD7402F3F338F79121C840AC0D98");
					singletonInstance.adView.loadAd(adRequestBuilder.build());
					Log.println(Log.INFO, "jni", "============= Finished Ad Startup ==============");
				}
			}
			);
		}
	}

	public void showAd()
	{
		singletonInstance.runOnUiThread(
			new Runnable()
			{
				@Override
				public void run()
				{
					if(!adsInitialized)
						return;
					popup.showAtLocation(mainLayout, Gravity.BOTTOM, 0, 0);
					popup.update();
				}
			}
		);
	}

	public void hideAd()
	{
		singletonInstance.runOnUiThread(
			new Runnable()
			{
				@Override
				public void run()
				{
					if(!adsInitialized)
						return;
					popup.dismiss();
				}
			}
		);
	}
	public void loge(String msg)
	{
		Log.println(Log.ERROR,"jni",msg);
	}
	public void logi(String msg)
	{
		Log.println(Log.INFO,"jni",msg);
	}

	public boolean test(int a, int b)
	{
		loge("test method called with parameters: (" + a + "," + b + ")");
		return (a==b);
	}

	@Override
	public void onDestroy()
	{
		if(adView != null)
		{
			adView.destroy();
		}
		super.onDestroy();
	}
}
