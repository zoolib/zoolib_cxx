// @formatter:off

package org.zoolib;

import android.app.Application;
import android.util.Log;
import android.opengl.GLSurfaceView;

import androidx.lifecycle.ViewModel;

import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;

// =================================================================================================
public class ViewModel_Game extends ViewModel
	implements GLSurfaceView.Renderer
	{
	private final String TAG = "ViewModel_Game";
	private int fWidth;
	private int fHeight;

	public ViewModel_Game()
		{
		Log.v(TAG, "ctor");

		String theString = spGetApplicationUsingReflection().getApplicationInfo().sourceDir;
		sInit(theString);
		}

// From ViewModel
	protected void onCleared()
		{
		Log.v(TAG, "onCleared");
		sTearDown();
		}

// From GLSurfaceView.Renderer
	public void onSurfaceCreated(GL10 gl, EGLConfig config)
		{
		sSurfaceAndContextIsFresh();
		}

	public void onDrawFrame(GL10 gl)
		{
		sDraw(fWidth, fHeight);
		}
	
	public void onSurfaceChanged(GL10 gl, int width, int height)
		{
		fWidth = width;
		fHeight = height;
		}

// Helper
	private static Application spGetApplicationUsingReflection()
		{
		try
			{
			return (Application) Class.forName("android.app.AppGlobals")
				.getMethod("getInitialApplication").invoke(null, (Object[]) null);
			}
		catch (Exception ex)
			{}
		return null;
		}

// JNI
	public static native void sInit(String iAPKPath);
	public static native void sSurfaceAndContextIsFresh();

	public static native void sPauseGameLoop();
	public static native void sResumeGameLoop();
	public static native void sTearDown();

	public static native void sOnTouch(int pointerId, int action, float x, float y, float p);

	public static native void sDraw(int iWidth, int iHeight);
	}

// =================================================================================================
