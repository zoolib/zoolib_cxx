// @formatter:off

package org.zoolib;

import android.app.Activity;
import android.app.Application;import android.content.Context;
import android.opengl.GLES20;
import android.opengl.GLSurfaceView;
import android.os.Bundle;
import android.view.View;

import androidx.appcompat.app.AppCompatActivity;
import androidx.lifecycle.ViewModel;
import androidx.lifecycle.ViewModelProviders;

import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;

import android.util.Log;

// =================================================================================================

public class Activity_Game
	extends AppCompatActivity
	{
	ViewModel_Game fViewModel;
//	static
//		{
//		System.loadLibrary("ZooLibGame");
//		}

	protected void onCreate(Bundle savedInstanceState)
		{
		super.onCreate(savedInstanceState);

		fViewModel = ViewModelProviders.of(this).get(ViewModel_Game.class);

		GLSurfaceView_Game theView = new GLSurfaceView_Game(this, fViewModel);

		setContentView(theView);
		}

	private void hideSystemUI()
		{
		// Set the IMMERSIVE flag.
		// Set the content to appear under the system bars so that the content
		// doesn't resize when the system bars hide and show.
		getWindow().getDecorView().setSystemUiVisibility(
				View.SYSTEM_UI_FLAG_LAYOUT_STABLE
						| View.SYSTEM_UI_FLAG_LAYOUT_HIDE_NAVIGATION
						| View.SYSTEM_UI_FLAG_LAYOUT_FULLSCREEN
						| View.SYSTEM_UI_FLAG_HIDE_NAVIGATION // hide nav bar
						| View.SYSTEM_UI_FLAG_FULLSCREEN // hide status bar
						| View.SYSTEM_UI_FLAG_IMMERSIVE);

//		getWindow().requestFeature(Window.FEATURE_ACTION_BAR);
//		getActionBar().hide();
		}

	public void onWindowFocusChanged(boolean hasFocus)
		{
		super.onWindowFocusChanged(hasFocus);
		if (hasFocus)
			this.hideSystemUI();
		}
	}

// =================================================================================================
class ViewModel_Game extends ViewModel
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
		sStart();
		}

// From GLSurfaceView.Renderer
	public void onSurfaceCreated(GL10 gl, EGLConfig config)
	{
		// We have a new GLContext.
		sSurfaceAndContextCreated();
	}

	public void onDrawFrame(GL10 gl)
	{
		// Redraw background color
		// GLES20.glClear(GLES20.GL_COLOR_BUFFER_BIT);
		sDraw(fWidth, fHeight);
	}

	public void onSurfaceChanged(GL10 gl, int width, int height)
	{
//		   GLES20.glViewport(0, 0, width, height);
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
		catch ()
			{}
		return null;
		}

// JNI
	public static native void sInit(String iAPKPath);
	public static native void sSurfaceAndContextCreated();

	public static native void sPauseGameLoop();
	public static native void sResumeGameLoop();
	public static native void sTearDown();

	public static native void sOnTouch(int pointerId, int action, float x, float y, float p);

	public static native void sDraw(int iWidth, int iHeight);
	}

// =================================================================================================

class GLSurfaceView_Game extends GLSurfaceView
{
	private final ViewModel_Game fViewModel;

	private static final int GL_RED_SIZE = 8;
	private static final int GL_GREEN_SIZE = 8;
	private static final int GL_BLUE_SIZE = 8;
	private static final int GL_ALPHA_SIZE = 0;
	private static final int GL_DEPTH_SIZE = 16;
	private static final int GL_STENCIL_SIZE = 0;

	private static final int GL_VERSION = 2;

	public GLSurfaceView_Game(Context context, ViewModel_Game iViewModel)
	{
		super(context);

		setEGLConfigChooser(GL_RED_SIZE, GL_GREEN_SIZE, GL_BLUE_SIZE, GL_ALPHA_SIZE, GL_DEPTH_SIZE, GL_STENCIL_SIZE);
		setEGLContextClientVersion(GL_VERSION);
		setDebugFlags(DEBUG_CHECK_GL_ERROR | DEBUG_LOG_GL_CALLS);

		fViewModel = iViewModel;

		setRenderer(iViewModel);

	    // Could instead be RENDERMODE_WHEN_DIRTY, in which case we
	    // do this.requestRender to trigger a callback.
	    setRenderMode(GLSurfaceView.RENDERMODE_CONTINUOUSLY);
	}
}
