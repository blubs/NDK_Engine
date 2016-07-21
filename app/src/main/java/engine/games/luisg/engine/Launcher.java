package engine.games.luisg.engine;

import android.app.NativeActivity;

import android.os.Bundle;
import android.util.Log;
//import com.google.android.gms.ads.*;
//import com.google.android.gms.ads.adview;
//import com.google.android.gms.ads.identifier.AdvertisingIdClient;

/**
 * Created by blubs on 7/19/2016.
 */

public class Launcher extends NativeActivity
{
	@Override
	public void onCreate(Bundle savedInstanceState)
	{
		super.onCreate(savedInstanceState);
		//Supposedly this should work, however the packages to use these libraries aren't recognized
//		AdView view = new AdView(this);
//		view.setAdUnitId("ca-app-pub-3940256099942544~3347511713");
//		view.setAdSize(AdSize.SMART_BANNER);

	}
	public boolean test(int a, int b)
	{
		return (a==b);
	}
}
