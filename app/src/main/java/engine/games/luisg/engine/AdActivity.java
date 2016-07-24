package engine.games.luisg.engine;

import android.app.Activity;
import android.content.Context;
import android.content.Intent;
import android.os.Bundle;
import android.widget.LinearLayout;

import android.view.View;
import android.view.ViewGroup;
import android.widget.LinearLayout;
//import com.google.android.gms.ads.*;
import com.google.android.gms.ads.AdRequest;
import com.google.android.gms.ads.AdView;
import com.google.android.gms.ads.AdSize;
/**
 * Created by blubs on 7/22/2016.
 */
public class AdActivity extends Activity
{
	@Override
	public void onCreate(Bundle savedInstanceState)
	{
		super.onCreate(savedInstanceState);

		LinearLayout layout = new LinearLayout(this);
		layout.setOrientation(LinearLayout.VERTICAL);

		AdView view = new AdView(this);

		//This is for a test ad
		view.setAdUnitId("ca-app-pub-3940256099942544/6300978111");
		view.setAdSize(AdSize.BANNER);

		AdRequest.Builder adRequestBuilder = new AdRequest.Builder();
		//adRequestBuilder.addTestDevice(AdRequest.DEVICE_ID_EMULATOR);
		adRequestBuilder.addTestDevice("BANANANAANANANANANANNANANANANANA");

		layout.addView(view);
		view.loadAd(adRequestBuilder.build());
		//view.setVisibility(View.VISIBLE);

		setContentView(layout);
	}
}
