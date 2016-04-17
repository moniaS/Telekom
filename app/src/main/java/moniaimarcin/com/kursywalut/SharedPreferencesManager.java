package moniaimarcin.com.kursywalut;

import android.content.Context;
import android.content.SharedPreferences;

/**
 * Created by lenovo on 2016-04-16.
 */
public abstract class SharedPreferencesManager {
    private static final String SP_NAME = "spWaluty";
    private static final String INPUT_CURRENCY_KEY = "inputCurrency";
    private static final String OUTPUT_CURRENCY_KEY = "outputCurrency";
    private static final String LAST_RATE_KEY = "lastRate";
    private static final String LAST_RATE_DATE_KEY = "lastRateDate";

    public static void saveLastInputCurrency(Context context, String currency) {
        SharedPreferences.Editor spEditor = getSharedPreferences(context).edit();
        spEditor.putString(INPUT_CURRENCY_KEY, currency);
        spEditor.apply();
    }

    public static void saveLastOutputCurrency(Context context, String currency) {
        SharedPreferences.Editor spEditor = getSharedPreferences(context).edit();
        spEditor.putString(OUTPUT_CURRENCY_KEY, currency);
        spEditor.apply();
    }

    public static void saveLastRate(Context context, double rate) {
        SharedPreferences.Editor spEditor = getSharedPreferences(context).edit();
        spEditor.putFloat(LAST_RATE_KEY, (float) rate);
        spEditor.apply();
    }

    public static void saveLastRateDate(Context context, String date) {
        SharedPreferences.Editor spEditor = getSharedPreferences(context).edit();
        spEditor.putString(LAST_RATE_DATE_KEY, date);
        spEditor.apply();
    }

    public static String getLastRateDate(Context context) {
        return getSharedPreferences(context).getString(LAST_RATE_DATE_KEY, "");
    }

    public static String getLastInputCurrency(Context context) {
        return getSharedPreferences(context).getString(INPUT_CURRENCY_KEY, "PLN");
    }

    public static String getLastOutputCurrency(Context context) {
        return getSharedPreferences(context).getString(OUTPUT_CURRENCY_KEY, "EUR");
    }

    public static double getLastRate(Context context) {
        return getSharedPreferences(context).getFloat(LAST_RATE_KEY, 0);
    }


    private static SharedPreferences getSharedPreferences(Context context) {
        return context.getSharedPreferences(SP_NAME, Context.MODE_PRIVATE);
    }
}
