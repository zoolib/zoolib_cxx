package org.zoolib;

import android.app.Activity;
import android.content.Context;
import android.content.pm.ActivityInfo;
import android.graphics.Canvas;
import android.graphics.PixelFormat;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.util.Log;
import android.view.KeyEvent;
import android.view.MotionEvent;
import android.view.SurfaceHolder;
import android.view.SurfaceView;
import android.view.View;
import android.view.Window;
import android.view.WindowManager;

import javax.microedition.khronos.egl.EGL10;
import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.egl.EGLContext;
import javax.microedition.khronos.egl.EGLDisplay;
import javax.microedition.khronos.egl.EGLSurface;

import static android.opengl.EGL14.EGL_CONTEXT_CLIENT_VERSION;

// =================================================================================================

public class SDLActivity
extends Activity
	{
	private static SDLActivity spSingleton;
	private static SDLSurface spSurface;

	static
		{
		System.loadLibrary("ZooLibGame");
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

	protected void onCreate(Bundle savedInstanceState)
		{
		super.onCreate(savedInstanceState);
		
		// So we can call stuff from static callbacks
		spSingleton = this;

		requestWindowFeature(Window.FEATURE_NO_TITLE);

		getWindow().setFlags
			(WindowManager.LayoutParams.FLAG_FULLSCREEN,
			WindowManager.LayoutParams.FLAG_FULLSCREEN);  

		setRequestedOrientation(ActivityInfo.SCREEN_ORIENTATION_LANDSCAPE);

		this.hideSystemUI();

		// Set up the surface
		spSurface = new SDLSurface(getApplication());
		setContentView(spSurface);
		SurfaceHolder holder = spSurface.getHolder();
		holder.setType(SurfaceHolder.SURFACE_TYPE_GPU);

		String theString = this.getApplicationInfo().sourceDir;
		sInit(theString);
		}

	public void onWindowFocusChanged(boolean hasFocus)
		{
		super.onWindowFocusChanged(hasFocus);
		if (hasFocus)
			this.hideSystemUI();
		}

	protected void onPause()
		{
		super.onPause();
		sOnPause();
		}

	protected void onResume()
		{
		super.onResume();
		sOnResume();
		}

	// Messages from the SDLMain thread
	static int COMMAND_CHANGE_TITLE = 1;

	// Handler for the messages
	Handler commandHandler = new Handler()
		{
		public void handleMessage(Message msg)
			{
			if (msg.arg1 == COMMAND_CHANGE_TITLE)
				{
				setTitle((String)msg.obj);
				}
			}
		};

	// Send a message from the SDLMain thread
	void sendCommand(int command, Object data)
		{
		Message msg = commandHandler.obtainMessage();
		msg.arg1 = command;
		msg.obj = data;
		commandHandler.sendMessage(msg);
		}

	public static void flipBuffers()
		{
		spSurface.flipEGL();
		}

	public static void setActivityTitle(String title)
		{
		// Called from SDLMain() thread and can't directly affect the view
		spSingleton.sendCommand(COMMAND_CHANGE_TITLE, title);
		}

	public static native void sInit(String iAPK);

	public static native void sRunLoop();
	public static native void sQuitLoop();

	public static native void sOnPause();
	public static native void sOnResume();
	public static native void sOnResize(int x, int y, int format);
	public static native void sOnKeyDown(int keycode);
	public static native void sOnKeyUp(int keycode);
	public static native void sOnTouch(int pointerId, int action, float x, float y, float p);
	public static native void sOnAccel(float x, float y, float z);
	
	} // class SDLActivity

// =================================================================================================
//	SDLSurface. This is what we draw on, so we need to know when it's created
//	in order to do anything useful. 
//	Because of this, that's where we set up the SDL thread

class SDLSurface
extends SurfaceView
implements SurfaceHolder.Callback, Runnable, View.OnKeyListener, View.OnTouchListener
	{
	// This is what SDL runs in. It invokes SDL_main(), eventually
	private Thread mSDLThread;
	
	// EGL private objects
	Context fContext;
	private EGLDisplay mEGLDisplay;
	private EGLContext mEGLContext;
	EGLConfig mFuckingConfig;
	private EGLSurface mEGLSurface;

	// Startup
	public SDLSurface(Context context)
		{
		super(context);
		fContext = context;
		getHolder().addCallback(this); 
	
		setFocusable(true);
		setFocusableInTouchMode(true);
		requestFocus();
		setOnKeyListener(this); 
		setOnTouchListener(this); 

		EGL10 egl = (EGL10)EGLContext.getEGL();

		mEGLDisplay = egl.eglGetDisplay(EGL10.EGL_DEFAULT_DISPLAY);

		egl.eglInitialize(mEGLDisplay, new int[2]);

		int[] configSpec =
			{
			EGL10.EGL_NONE
			};

		EGLConfig[] configs = new EGLConfig[1];
		int[] num_config = new int[1];
		boolean bChoose = egl.eglChooseConfig(mEGLDisplay, configSpec, configs, 1, num_config);

		boolean bConfig0 = num_config[0] == 0;

		mFuckingConfig = configs[0];

		int[] attrib_list = { EGL_CONTEXT_CLIENT_VERSION, 2, EGL10.EGL_NONE };

		mEGLContext = egl.eglCreateContext(mEGLDisplay, mFuckingConfig, EGL10.EGL_NO_CONTEXT, attrib_list);
		}

	// Called when we have a valid drawing surface
	public void surfaceCreated(SurfaceHolder holder)
		{
		Log.v("SDL", "surfaceCreated()");

		this.updateEGL();
		}

	// Called when we lose the surface
	public void surfaceDestroyed(SurfaceHolder holder)
		{
		Log.v("SDL", "surfaceDestroyed()");
		mEGLSurface = null;

		// Send a quit message to the application
		SDLActivity.sQuitLoop();

		// Now wait for the SDL thread to quit
		if (mSDLThread != null)
			{
			try
				{
				mSDLThread.join();
				}
			catch (Exception e)
				{
				Log.v("SDL", "Problem stopping thread: " + e);
				}
			mSDLThread = null;

			Log.v("SDL", "Finished waiting for SDL thread");
			}
		}

	// Called when the surface is resized
	public void surfaceChanged(SurfaceHolder holder,
		int format, int width, int height)
		{
		int sdlFormat = 0x85151002; // SDL_PIXELFORMAT_RGB565 by default
		switch (format)
			{
			case PixelFormat.A_8:
				Log.v("SDL", "pixel format A_8");
				break;
			case PixelFormat.LA_88:
				Log.v("SDL", "pixel format LA_88");
				break;
			case PixelFormat.L_8:
				Log.v("SDL", "pixel format L_8");
				break;
			case PixelFormat.RGBA_4444:
				Log.v("SDL", "pixel format RGBA_4444");
				sdlFormat = 0x85421002; // SDL_PIXELFORMAT_RGBA4444
				break;
			case PixelFormat.RGBA_5551:
				Log.v("SDL", "pixel format RGBA_5551");
				sdlFormat = 0x85441002; // SDL_PIXELFORMAT_RGBA5551
				break;
			case PixelFormat.RGBA_8888:
				Log.v("SDL", "pixel format RGBA_8888");
				sdlFormat = 0x86462004; // SDL_PIXELFORMAT_RGBA8888
				break;
			case PixelFormat.RGBX_8888:
				Log.v("SDL", "pixel format RGBX_8888");
				sdlFormat = 0x86262004; // SDL_PIXELFORMAT_RGBX8888
				break;
			case PixelFormat.RGB_332:
				Log.v("SDL", "pixel format RGB_332");
				sdlFormat = 0x84110801; // SDL_PIXELFORMAT_RGB332
				break;
			case PixelFormat.RGB_565:
				Log.v("SDL", "pixel format RGB_565");
				sdlFormat = 0x85151002; // SDL_PIXELFORMAT_RGB565
				break;
			case PixelFormat.RGB_888:
				Log.v("SDL", "pixel format RGB_888");
				// Not sure this is right, maybe SDL_PIXELFORMAT_RGB24 instead?
				sdlFormat = 0x86161804; // SDL_PIXELFORMAT_RGB888
				break;
			default:
				Log.v("SDL", "pixel format unknown " + format);
				break;
			}
		SDLActivity.sOnResize(width, height, sdlFormat);

		// Now start up the C app thread
		if (mSDLThread == null)
			{
			mSDLThread = new Thread(this, "SDLRenderThread");
			mSDLThread.start(); 
			}
		}

	public void run()
		{
		EGL10 egl = (EGL10)EGLContext.getEGL();
		if (!egl.eglMakeCurrent(mEGLDisplay, mEGLSurface, mEGLSurface, mEGLContext))
			{
			Log.e("SDL", "Couldn't make context current");
			}

		SDLActivity.sRunLoop();
		}

	// unused
	public void onDraw(Canvas canvas)
		{}


	// EGL functions
	public void updateEGL()
		{
		EGL10 egl = (EGL10)EGLContext.getEGL();

		EGLSurface surface = egl.eglCreateWindowSurface(mEGLDisplay, mFuckingConfig , this, null);
		if (surface == EGL10.EGL_NO_SURFACE)
			{
			Log.e("SDL", "Couldn't create surface");
			return;
			}

		mEGLSurface = surface;
		}

	// EGL buffer flip
	public void flipEGL()
		{
		try
			{
			EGL10 egl = (EGL10) EGLContext.getEGL();

			egl.eglWaitNative(EGL10.EGL_NATIVE_RENDERABLE, null);

			egl.eglWaitGL();

			egl.eglSwapBuffers(mEGLDisplay, mEGLSurface);
			}
		catch (Exception e)
			{
			Log.v("SDL", "flipEGL(): " + e);
			for (StackTraceElement s : e.getStackTrace())
				{
				Log.v("SDL", s.toString());
				}
			}
		}

	// Key events
	public boolean onKey(View v, int keyCode, KeyEvent event)
		{
		if (event.getAction() == KeyEvent.ACTION_DOWN)
			{
			SDLActivity.sOnKeyDown(keyCode);
			return true;
			}
		else if (event.getAction() == KeyEvent.ACTION_UP)
			{
			SDLActivity.sOnKeyUp(keyCode);
			return true;
			}
		
		return false;
		}

	// Touch events
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
				SDLActivity.sOnTouch(pointerId, MotionEvent.ACTION_DOWN,
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
					SDLActivity.sOnTouch(event.getPointerId(ii), MotionEvent.ACTION_MOVE,
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
				SDLActivity.sOnTouch(pointerId, MotionEvent.ACTION_UP,
					event.getX(pointerIndex),
					event.getY(pointerIndex),
					event.getPressure(pointerIndex));
	            break;
				}
	        }
		return true;
		}
	} // class SDLSurface
