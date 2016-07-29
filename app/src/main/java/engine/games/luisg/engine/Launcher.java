package engine.games.luisg.engine;

import android.app.NativeActivity;

import android.content.Context;
import android.content.Intent;
import android.os.Bundle;
import android.util.Log;
//import com.google.android.gms.ads.identifier.AdvertisingIdClient;

/**
 * Created by blubs on 7/19/2016.
 */

public class Launcher extends NativeActivity
{
	public static Launcher singletonInstance;

	public AdActivity adActivity;

	@Override
	public void onCreate(Bundle savedInstanceState)
	{
		super.onCreate(savedInstanceState);

		Log.println(Log.INFO, "tag", "about to set instance for launcher");
		singletonInstance = this;
		Log.println(Log.INFO,"tag","set instance for launcher");

//		Context context = this.getApplication().getApplicationContext();

		Intent openAdActivity = new Intent(this,AdActivity.class);
		startActivity(openAdActivity);

		Log.println(Log.INFO, "tag", "code after startActivity was called");
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
}
