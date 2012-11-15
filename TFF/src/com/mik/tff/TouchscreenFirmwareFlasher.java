package com.mik.tff;

import java.io.BufferedReader;
import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.io.OutputStream;

import android.os.AsyncTask;
import android.os.Bundle;
import android.os.Handler;
import android.os.PowerManager;
import android.animation.Animator;
import android.animation.AnimatorSet;
import android.animation.ObjectAnimator;
import android.app.Activity;
import android.app.ProgressDialog;
import android.graphics.BitmapFactory;
import android.graphics.Rect;
import android.util.Log;
import android.view.GestureDetector;
import android.view.MotionEvent;
import android.view.View;
import android.view.ViewGroup.LayoutParams;
import android.view.Window;
import android.view.WindowManager;
import android.view.animation.Animation;
import android.view.animation.Transformation;
import android.widget.CheckBox;
import android.widget.CompoundButton;
import android.widget.ImageView;
import android.widget.LinearLayout;
import android.widget.RelativeLayout;
import android.widget.Switch;
import android.widget.TextView;
import android.widget.Toast;

public class TouchscreenFirmwareFlasher extends Activity {
	private static int WALL_ANIMATION = 500;
	private static String TAG = "TouchscreenFirmwareFlasher";
	private static float BLINKING_TOP = 0.27f;
	private static float BLINKING_BOTTOM = 0.1f;
	private static int ALARM_ANIMATION = 400;
	private static String FIRMWARE_FILE = "/system/etc/firmware/touch_fw";
	private static String FT5X06_FWUPDATE = "/sys/devices/platform/omap/omap_i2c.2/i2c-2/2-0038/fwupdate";
	private static String FT5X06_WMREG = "/sys/devices/platform/omap/omap_i2c.2/i2c-2/2-0038/wmreg";
	private static String FT5X06_WMVAL = "/sys/devices/platform/omap/omap_i2c.2/i2c-2/2-0038/wmval";
	private static String FW_VERSION_REG = "0xA6";
	private static String FW_STOCK_VALUE = "13|14";
	private static String FW_NEW_VALUE = "0b";

	ImageView mImage;
	ImageView mButton;
	View mWall;
	Animator mCurrentWallAnimator = null;
	boolean mLongClick = false;
	Handler mHandler = new Handler();
	Handler mLongClickHandler = new Handler();
	LinearLayout mNewFirmwareVersionLayout;
	TextView mCurrentFirmwareVersion;
	TextView mNewFirmwareVersion;
	CheckBox mSwitch;
	RelativeLayout mFirmwarelayout;
	ImageView mSeparator;
	GestureDetector gd;
	int mScrollDownSize;
	int mFirmwareToFlash;
	TextView mManual;

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        requestWindowFeature(Window.FEATURE_NO_TITLE);
        getWindow().setFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN,
                WindowManager.LayoutParams.FLAG_FULLSCREEN);
        setContentView(R.layout.activity_touchscreen_firmware_flasher);
        
        mScrollDownSize = BitmapFactory.decodeResource(getResources(), R.drawable.back).getHeight();
        
        mImage = (ImageView) findViewById(R.id.imageView1);
        mButton = (ImageView) findViewById(R.id.big_red_button);
        mWall = findViewById(R.id.wall);
        mNewFirmwareVersionLayout = (LinearLayout) findViewById(R.id.new_firmware_version_layout);
        mCurrentFirmwareVersion = (TextView) findViewById(R.id.current_firmware);
        mNewFirmwareVersion = (TextView) findViewById(R.id.new_firmware);
        mSwitch = (CheckBox) findViewById(R.id.switch1);
        mFirmwarelayout = (RelativeLayout) findViewById(R.id.firmware_id_layout);
        mSeparator = (ImageView) findViewById(R.id.separator);
        mManual = (TextView) findViewById(R.id.manual);

        mButton.setOnTouchListener(new View.OnTouchListener() {
        	Rect mRect;
        	boolean mIgnoreTouches = false;
			
			public boolean onTouch(View v, MotionEvent event) {
				Log.v(TAG, "onTouch()");
				if (mButton.getAlpha() != 1) {
					return false;
				}
				switch(event.getAction()) {
				case MotionEvent.ACTION_DOWN:
					mRect = new Rect(0, 0, (int)mButton.getWidth(), (int)mButton.getHeight());
					animateWallIn();
					mLongClickHandler.postDelayed(mLongClickRunnable, WALL_ANIMATION*2);
					mLongClick = false;
					break;
				case MotionEvent.ACTION_UP:
					if (!mIgnoreTouches && !mLongClick) {
						Toast.makeText(TouchscreenFirmwareFlasher.this, "Long Press Red Button", Toast.LENGTH_SHORT).show();
					}
					if (!mLongClick) {
						animateWallOut();
						mLongClickHandler.removeCallbacks(mLongClickRunnable);
					} else {
						doFlash();
						blinkingStart();
						animateButtonOut();
					}
					mIgnoreTouches = false;
					break;
				case MotionEvent.ACTION_MOVE:
					if (!mRect.contains((int) event.getX(), (int) event.getY()) && !mIgnoreTouches) {
						animateWallOut();
						mIgnoreTouches = true;
						mLongClickHandler.removeCallbacks(mLongClickRunnable);
						mLongClick = false;
					}
				}
				return true;
			}
		});

        mSwitch.setOnCheckedChangeListener(new CompoundButton.OnCheckedChangeListener() {
			
			public void onCheckedChanged(CompoundButton buttonView, boolean isChecked) {
				if (isChecked) {
					animateButtonIn();
				} else {
					animateButtonOut();
				}
			}
		});
        
        gd = new GestureDetector(this, new GestureDetector.OnGestureListener() {
			
			public boolean onSingleTapUp(MotionEvent e) {
				return false;
			}
			
			public void onShowPress(MotionEvent e) {
			}
			
			public boolean onScroll(MotionEvent e1, MotionEvent e2, float distanceX,
					float distanceY) {
				return false;
			}
			
			public void onLongPress(MotionEvent e) {
			}
			
			public boolean onFling(MotionEvent e1, MotionEvent e2, float velocityX,
					float velocityY) {
				if (mImage.getLayoutParams().height != mScrollDownSize) {
					if (velocityY <= 0) {
						HeightAnimation a = new HeightAnimation(true);
						a.setDuration(300);
						mImage.startAnimation(a);
					} else {
						HeightAnimation a = new HeightAnimation(false);
						a.setDuration(300);
						mImage.startAnimation(a);
					}
				}
				return true;
			}
			
			public boolean onDown(MotionEvent e) {
				return false;
			}
		});
        
        mSeparator.setOnTouchListener(new View.OnTouchListener() {
        	float mPos = -1;

			public boolean onTouch(View v, MotionEvent event) {
				MotionEvent ev = MotionEvent.obtain(event.getDownTime(), event.getEventTime(), event.getAction(), event.getRawX(), event.getRawY(), event.getPressure(), event.getSize(), event.getMetaState(), event.getXPrecision(), event.getYPrecision(), event.getDeviceId(), event.getEdgeFlags());
				boolean res = gd.onTouchEvent(ev);
				switch(event.getAction()) {
				case MotionEvent.ACTION_DOWN:
					mPos = event.getRawY();
					mSeparator.setImageResource(R.drawable.status_bar_close_on);
					break;
				case MotionEvent.ACTION_MOVE:
					float diff = event.getRawY() - mPos;
					LayoutParams lp =  mImage.getLayoutParams();
					if (lp.height + diff > mScrollDownSize && lp.height < mScrollDownSize) {
						diff = mScrollDownSize - lp.height;
					}
					if (mPos != -1 && lp.height + diff >= 0 && lp.height + diff <= mScrollDownSize) {
						lp.height += diff;
						mImage.requestLayout();
						mPos += diff;
					}
					break;
				case MotionEvent.ACTION_UP:
					mPos = -1;
					mSeparator.setImageResource(R.drawable.status_bar_close_off);
					if (!res) {
						HeightAnimation a = new HeightAnimation(true);
						a.setDuration(300);
						mImage.startAnimation(a);
					}
				}
				return true;
			}
		});

        new FirmwareDetector().execute();
    }
    
    Runnable mLongClickRunnable = new Runnable() {
		
		public void run() {
			Log.v(TAG, "mLongClickRunnable()");
			mLongClick = true;
		}
	};
    
    private void animateWallIn() {
    	Log.v(TAG, "animateWalIn()");
    	finishWallAnimator();
    	AnimatorSet set = new AnimatorSet();
    	set.setDuration(WALL_ANIMATION);

    	Animator red_lights = ObjectAnimator.ofFloat(mWall, "alpha", mWall.getAlpha(), 0.2f);
    	Animator manual_anim = ObjectAnimator.ofFloat(mManual, "alpha", mManual.getAlpha(), 1);
    	Animator fw_layout = ObjectAnimator.ofFloat(mFirmwarelayout, "alpha", mFirmwarelayout.getAlpha(), 0);
    	Animator switch_anim = ObjectAnimator.ofFloat(mSwitch, "alpha", mSwitch.getAlpha(), 0);
    	Animator image_anim = ObjectAnimator.ofFloat(mImage, "alpha", mImage.getAlpha(), 0);
    	Animator separator_anim = ObjectAnimator.ofFloat(mSeparator, "alpha", mImage.getAlpha(), 0);
    	
    	AnimatorSet set1 = new AnimatorSet();
    	set1.playTogether(fw_layout, switch_anim, image_anim, separator_anim);
    	set1.addListener(new Animator.AnimatorListener() {
			
			public void onAnimationStart(Animator animation) {
			}
			
			public void onAnimationRepeat(Animator animation) {
			}
			
			public void onAnimationEnd(Animator animation) {
				mFirmwarelayout.setVisibility(View.INVISIBLE);
				mSwitch.setVisibility(View.INVISIBLE);
				mImage.setVisibility(View.INVISIBLE);
			}
			
			public void onAnimationCancel(Animator animation) {
			}
		});
    	AnimatorSet set2 = new AnimatorSet();
    	set2.playTogether(red_lights, manual_anim);
    	set.playSequentially(set1, set2);
    	
    	set.start();

		mCurrentWallAnimator = set;
    }

    private void animateWallOut() {
    	Log.v(TAG, "animateWallOut()");

    	finishWallAnimator();
    	AnimatorSet set = new AnimatorSet();
    	set.setDuration(WALL_ANIMATION);

    	Animator red_lights = ObjectAnimator.ofFloat(mWall, "alpha", mWall.getAlpha(), 0);
    	Animator manual_anim = ObjectAnimator.ofFloat(mManual, "alpha", mManual.getAlpha(), 0);
    	Animator fw_layout = ObjectAnimator.ofFloat(mFirmwarelayout, "alpha", mFirmwarelayout.getAlpha(), 1);
    	Animator switch_anim = ObjectAnimator.ofFloat(mSwitch, "alpha", mSwitch.getAlpha(), 1);
    	Animator image_anim = ObjectAnimator.ofFloat(mImage, "alpha", mImage.getAlpha(), 1);
    	Animator separator_anim = ObjectAnimator.ofFloat(mSeparator, "alpha", mImage.getAlpha(), 1);
    	
    	set.playTogether(red_lights, fw_layout, switch_anim, image_anim, separator_anim, manual_anim);
    	
		mFirmwarelayout.setVisibility(View.VISIBLE);
		mSwitch.setVisibility(View.VISIBLE);
		mImage.setVisibility(View.VISIBLE);
    	
    	set.start();

		mCurrentWallAnimator = set;
    }
    
    private void finishWallAnimator() {
    	Log.v(TAG, "finishWallAnimator()");
    	if (mCurrentWallAnimator != null && mCurrentWallAnimator.isRunning()) {
    		mCurrentWallAnimator.cancel();
    	}
    }
    
    private void animateNewFirmwareVersionIn() {
    	Log.v(TAG, "animateFirmwareVersionIn()");
    	Animator a = ObjectAnimator.ofFloat(mNewFirmwareVersionLayout, "alpha", 0, 1);
    	a.setDuration(200);
    	a.start();
    }
    
    private void animateSwitchIn() {
    	Log.v(TAG, "animateSwitchIn()");
    	Animator a = ObjectAnimator.ofFloat(mSwitch, "alpha", 0, 1);
    	a.setDuration(200);
    	mSwitch.setVisibility(View.VISIBLE);
    	a.start();
    }
    
    private void animateButtonIn() {
    	Log.v(TAG, "animateButtonIn()");
    	Animator a = ObjectAnimator.ofFloat(mButton, "alpha", mButton.getAlpha(), 1);
    	a.setDuration(200);
    	a.start();
    }
    
    private void animateButtonOut() {
    	Log.v(TAG, "animateButtonOut()");
    	Animator a = ObjectAnimator.ofFloat(mButton, "alpha", mButton.getAlpha(), 0);
    	a.setDuration(200);
    	a.start();
    }
    
    private void doFlash() {
    	Log.v(TAG, "doFlash()");
    	try {
			extractFirmware(mFirmwareToFlash);
		} catch (IOException e) {
			e.printStackTrace();
			return;
		}
    	new FlashTask().execute();
    	
    }
    
    private void blinkingStart() {
    	Log.v(TAG, "blinkingStart()");
    	ObjectAnimator first_stage = ObjectAnimator.ofFloat(mWall, "alpha", mWall.getAlpha(), BLINKING_BOTTOM);
    	first_stage.setDuration(100);

    	final ObjectAnimator repeatable = ObjectAnimator.ofFloat(mWall, "alpha", BLINKING_BOTTOM, BLINKING_TOP);
    	repeatable.setDuration(ALARM_ANIMATION);
    	repeatable.setRepeatCount(ObjectAnimator.INFINITE);
    	repeatable.setRepeatMode(ObjectAnimator.REVERSE);

    	first_stage.addListener(new Animator.AnimatorListener() {
			
			public void onAnimationStart(Animator animation) {
			}
			
			public void onAnimationRepeat(Animator animation) {
			}
			
			public void onAnimationEnd(Animator animation) {
				repeatable.start();
			}
			
			public void onAnimationCancel(Animator animation) {
			}
		});
    	
    	first_stage.start();
    }
    
    class HeightAnimation extends Animation {
    	int mInitialHeight;
    	boolean mHide;
    	public HeightAnimation(boolean hide) {
    		mHide = hide;
		}
    	@Override
        protected void applyTransformation(float interpolatedTime, Transformation t) {
            int newHeight;

            if (mHide) {
            	newHeight = (int)(mInitialHeight * (1-interpolatedTime));
            } else {
            	newHeight = (int)(mInitialHeight + (mScrollDownSize - mInitialHeight) * interpolatedTime);
            }
            mImage.getLayoutParams().height = newHeight;
            mImage.requestLayout();
        }
    	
    	@Override
        public void initialize(int width, int height, int parentWidth, int parentHeight) {
            super.initialize(width, height, parentWidth, parentHeight);
            mInitialHeight = height;
        }
    	
    	@Override
        public boolean willChangeBounds() {
            return true;
        }
    }
    
    private void extractFirmware(int resId) throws IOException {
    	InputStream s = getResources().openRawResource(resId);
    	File tmp = File.createTempFile("mik", "fw");
    	OutputStream out = new FileOutputStream(tmp);
    	byte buf[]=new byte[1024];
    	int len;
    	while((len=s.read(buf))>0) {
    		out.write(buf,0,len);
    	}
    	out.close();
    	s.close();
    	remountSystem(true);
    	executeWithRoot("cp "+tmp.getAbsolutePath()+" "+FIRMWARE_FILE);
    	executeWithRoot("chmod 0444 "+FIRMWARE_FILE);
    	remountSystem(false);
    	tmp.delete();
    }
    
    private String executeWithRoot(String cmd) {
    	StringBuilder builder = new StringBuilder();
    	String line = new String();
    	try {
    		ProcessBuilder p_builder = new ProcessBuilder("su");
    		p_builder.redirectErrorStream(true);
			Process p = p_builder.start();
			p.getOutputStream().write((cmd+"\n").getBytes());
			p.getOutputStream().write("exit\n".getBytes());
			p.waitFor();
			InputStream input = p.getInputStream();
			BufferedReader reader = new BufferedReader(new InputStreamReader(input));
			while ((line=reader.readLine()) != null) {
				builder.append(line);
			}
			return builder.toString();
		} catch (IOException e) {
			e.printStackTrace();
		} catch (InterruptedException e) {
			e.printStackTrace();
		}
    	return "";
    }
    
    private void remountSystem(boolean rw) {
    	if(rw) {
    		executeWithRoot("mount -o remount,rw /system\n");
    	} else {
    		executeWithRoot("mount -o remount,ro /system\n");
    	}
    }
    
    class FlashTask extends AsyncTask<Void, String, Void> {
    	ProgressDialog dialog;
    	@Override
    	protected void onPreExecute() {
    		super.onPreExecute();
    		dialog = ProgressDialog.show(TouchscreenFirmwareFlasher.this, "Flashing firmware", "Flashing...");
    		dialog.setCancelable(false);
    	}
    	
    	@Override
    	protected void onProgressUpdate(String... values) {
    		super.onProgressUpdate(values);
    		dialog.setMessage(values[values.length-1]);
    	}

		@Override
		protected Void doInBackground(Void... params) {
			executeWithRoot("echo touch_fw > "+FT5X06_FWUPDATE);
			return null;
		}
		
		@Override
		protected void onPostExecute(Void result) {
			super.onPostExecute(result);
			dialog.dismiss();
			PowerManager pm = (PowerManager) getSystemService(POWER_SERVICE);
			pm.reboot("");
		}    	
    }
    
    class FirmwareDetector extends AsyncTask<Void, String, String> {
    	
    	@Override
    	protected void onProgressUpdate(String... values) {
    		super.onProgressUpdate(values);
    	}

		@Override
		protected String doInBackground(Void... arg0) {
			Log.i(TAG, "FirmwareDetector: writing...");
			executeWithRoot("echo "+FW_VERSION_REG+" > "+FT5X06_WMREG);
			Log.i(TAG, "FirmwareDetector: reading...");
			String res = executeWithRoot("cat "+FT5X06_WMVAL);
			Log.i(TAG, "FirmwareDetector: " + res);
			return res;
		}
    	
		@Override
		protected void onPostExecute(String res) {
			super.onPostExecute(res);
			if (res.isEmpty()) {
				mCurrentFirmwareVersion.setText("error detecting, flashing denied");
				return;
			}
			if (res.contains("0x0B")) {
				mCurrentFirmwareVersion.setText("10-touch");
				mNewFirmwareVersion.setText("stock");
				mFirmwareToFlash = R.raw.fts0019u700_ver14_app;
			} else if (res.contains("13") || res.contains("14")) {
				mCurrentFirmwareVersion.setText("stock");
				mNewFirmwareVersion.setText("10-touch");
				mFirmwareToFlash = R.raw.ft5406_sc3052_1024x768;
			} else {
				mCurrentFirmwareVersion.setText("unknown firmware version");
				mNewFirmwareVersion.setText("stock");
				mFirmwareToFlash = R.raw.fts0019u700_ver14_app;
			}
			animateNewFirmwareVersionIn();
			animateSwitchIn();
		}
    }
}
