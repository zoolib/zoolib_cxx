// @formatter:off

package org.zoolib;

import android.os.Vibrator;
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
	private final long fNative;

	public ViewModel_Game()
		{
		Log.v(TAG, "ctor");

		Application theApplication = spGetApplicationUsingReflection();
		String theString = theApplication.getApplicationInfo().sourceDir;

		fNative = nspInit(theString);
		}

	@Override // From ViewModel
	protected void onCleared()
		{
		Log.v(TAG, "onCleared");
		npTearDown(fNative);
		}

	@Override  // From GLSurfaceView.Renderer
	public void onSurfaceCreated(GL10 gl, EGLConfig config)
		{
		npSurfaceAndContextIsFresh(fNative);
		}

	@Override
	public void onDrawFrame(GL10 gl)
		{
		npDraw(fNative, fWidth, fHeight);
		}
	
	@Override
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


	public void attachActivity(Activity_Game iActivity)
		{
		}

// JNI
	private static native long nspInit(String iAPKPath);
	private native void npTearDown(long iNative);

	private native void npSurfaceAndContextIsFresh(long iNative);

	private native void npPauseGameLoop(long iNative);
	public void pauseGameLoop() { this.npPauseGameLoop(fNative); }

	private native void npResumeGameLoop(long iNative);
	public void resumeGameLoop() { this.npResumeGameLoop(fNative); }

	private native void npOnTouch(long iNative,int pointerId, int action, float x, float y, float p);
	public void onTouch(int pointerId, int action, float x, float y, float p)
		{ this.npOnTouch(fNative, pointerId, action, x, y, p); }

	private native void npDraw(long iNative, int iWidth, int iHeight);
	}

// =================================================================================================
