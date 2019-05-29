// @formatter:off

package org.zoolib;

import android.content.Context;
import android.content.pm.ActivityInfo;
import android.opengl.GLSurfaceView;
import android.os.Bundle;
import android.view.MotionEvent;
import android.view.View;

import androidx.appcompat.app.AppCompatActivity;
import androidx.lifecycle.ViewModelProviders;

//import android.util.Log;
import android.view.Window;
import android.view.WindowManager;

// =================================================================================================

public class Activity_Game
	extends AppCompatActivity
	{
	static
		{
		System.loadLibrary("ZooLibGame");
		}

	ViewModel_Game fViewModel;

// From Activity
	protected void onCreate(Bundle savedInstanceState)
		{
		super.onCreate(savedInstanceState);

		requestWindowFeature(Window.FEATURE_NO_TITLE);

		getWindow().setFlags
			(WindowManager.LayoutParams.FLAG_FULLSCREEN,
			WindowManager.LayoutParams.FLAG_FULLSCREEN);  

		setRequestedOrientation(ActivityInfo.SCREEN_ORIENTATION_LANDSCAPE);

		this.hideSystemUI();

		fViewModel = ViewModelProviders.of(this).get(ViewModel_Game.class);

		GLSurfaceView_Game theView = new GLSurfaceView_Game(this, fViewModel);

		setContentView(theView);
		}

	protected void onPause()
		{
		fViewModel.sPauseGameLoop();
		super.onPause();
		}

	protected void onResume()
		{
		super.onResume();
		fViewModel.sResumeGameLoop();
		}

	public void onWindowFocusChanged(boolean hasFocus)
		{
		super.onWindowFocusChanged(hasFocus);
		if (hasFocus)
			this.hideSystemUI();
		}

// Our protocol
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

	}

// =================================================================================================

class GLSurfaceView_Game extends GLSurfaceView
	implements View.OnTouchListener
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

		setOnTouchListener(this);

//		setFocusable(true);
//		setFocusableInTouchMode(true);
//		requestFocus();

		setEGLConfigChooser(GL_RED_SIZE, GL_GREEN_SIZE, GL_BLUE_SIZE, GL_ALPHA_SIZE, GL_DEPTH_SIZE, GL_STENCIL_SIZE);
		setEGLContextClientVersion(GL_VERSION);
		setDebugFlags(DEBUG_CHECK_GL_ERROR | DEBUG_LOG_GL_CALLS);

		fViewModel = iViewModel;

		setRenderer(iViewModel);

	    // Could instead be RENDERMODE_WHEN_DIRTY, in which case we
	    // do this.requestRender to trigger a callback.
	    setRenderMode(GLSurfaceView.RENDERMODE_CONTINUOUSLY);
	    }


// From View.OnTouchListener
   	public boolean onTouch(View v, MotionEvent event)
		{
        int pointerIndex = event.getActionIndex();
        int pointerId = event.getPointerId(pointerIndex);
        int maskedAction = event.getActionMasked();

        switch (maskedAction)
        	{
			case MotionEvent.ACTION_DOWN:
			case MotionEvent.ACTION_POINTER_DOWN:
				{
				fViewModel.sOnTouch(pointerId, MotionEvent.ACTION_DOWN,
					event.getX(pointerIndex),
					event.getY(pointerIndex),
					event.getPressure(pointerIndex));
	            break;
	            }
	        case MotionEvent.ACTION_MOVE:
    	    	{
        		// a pointer was moved
				for (int size = event.getPointerCount(), ii = 0; ii < size; ++ii)
					{
					fViewModel.sOnTouch(event.getPointerId(ii), MotionEvent.ACTION_MOVE,
						event.getX(ii),
						event.getY(ii),
						event.getPressure(ii));
					}
				break;
				}
			case MotionEvent.ACTION_UP:
			case MotionEvent.ACTION_POINTER_UP:
			case MotionEvent.ACTION_CANCEL:
				{
				fViewModel.sOnTouch(pointerId, MotionEvent.ACTION_UP,
					event.getX(pointerIndex),
					event.getY(pointerIndex),
					event.getPressure(pointerIndex));
	            break;
				}
	        }
		return true;
		}
	}
