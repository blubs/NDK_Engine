package engine.games.luisg.engine;

import android.app.ActionBar;
import android.app.Activity;
import android.content.Context;
import android.content.Intent;
import android.graphics.Color;
import android.os.Bundle;
import android.os.Handler;
import android.util.Log;
import android.view.Gravity;
import android.widget.LinearLayout;

import android.view.View;
import android.view.ViewGroup;
import android.widget.LinearLayout;
import android.widget.RelativeLayout;
//import com.google.android.gms.ads.*;
import com.google.android.gms.ads.AdRequest;
import com.google.android.gms.ads.AdView;
import com.google.android.gms.ads.AdSize;
/**
 * Created by blubs on 7/22/2016.
 */
public class AdActivity extends Activity
{
	public static AdActivity singletonInstance;

	public Launcher launcher;
	AdView adView;
	@Override
	public void onCreate(Bundle savedInstanceState)
	{
		super.onCreate(savedInstanceState);

		singletonInstance = this;

		//Because we know the Launcher activity starts first, the instance is set
		launcher = Launcher.singletonInstance;
		//Passing a reference to this instance to it as well
		launcher.adActivity = this;


		RelativeLayout layout = new RelativeLayout(this);

		RelativeLayout.LayoutParams params = new RelativeLayout.LayoutParams(RelativeLayout.LayoutParams.WRAP_CONTENT,RelativeLayout.LayoutParams.WRAP_CONTENT);

		params.addRule(RelativeLayout.ALIGN_PARENT_BOTTOM);

		adView = new AdView(this);

		//Only covers the area immediately behind the ad, not entirely useful
		//view.setBackgroundColor(Color.GRAY);


		layout.addView(adView, params);

		//This is for a test ad
		adView.setAdSize(AdSize.BANNER);
		adView.setAdUnitId("ca-app-pub-3940256099942544/6300978111");
		//This is for using my actual ad unit id
		//view.setAdUnitId(DevData.adUnitId);
		//adRequestBuilder.addTestDevice(AdRequest.DEVICE_ID_EMULATOR);
		//Supposedly for test ads
		//adRequestBuilder.addTestDevice("BANANANAANANANANANANNANANANANANA");
		//Log recommends using this to get test ads

		//Deferring the following code due to delayed startup
		final Handler handler = new Handler();
		handler.postDelayed(new Runnable()
		{
			@Override
			public void run()
			{
				Log.println(Log.INFO, "jni", "============= Initiating Ad Startup ==============");
				AdRequest.Builder adRequestBuilder = new AdRequest.Builder();
				adRequestBuilder.addTestDevice("3218CD7402F3F338F79121C840AC0D98");
				adView.loadAd(adRequestBuilder.build());
				adView.bringToFront();
				Log.println(Log.INFO, "jni", "============= Finished Ad Startup ==============");
			}
		}, 5000);

		launcher.loge("TEST FROM ADACTIVITY HAHAHAHA");

/*
		//((GridLayout.LayoutParams)button.getLayoutParams()).setGravity(int)
		//view.setVisibility(View.VISIBLE);
*/
		setContentView(layout);
	}
}
