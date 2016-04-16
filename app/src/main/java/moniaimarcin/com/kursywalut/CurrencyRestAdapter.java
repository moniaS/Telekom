package moniaimarcin.com.kursywalut;

import retrofit.Callback;
import retrofit.RestAdapter;

/**
 * Created by Marcin Szalek on 2016-02-29.
 */
public class CurrencyRestAdapter {
    protected CurrencyApi mApi;
    static final String SERVER_URL = "http://api.fixer.io";

    public CurrencyRestAdapter() {
        RestAdapter mRestAdapter = new RestAdapter.Builder()
                .setLogLevel(RestAdapter.LogLevel.FULL)
                .setEndpoint(SERVER_URL)
                .build();
        mApi = mRestAdapter.create(CurrencyApi.class);
    }

    public void getAvailableCurrencies(Callback<CurrencyResponse> callback) {
        mApi.getAvailableCurrencies(callback);
    }

    public void getRate(String inputCurrency, String outputCurrency, Callback<CurrencyResponse> callback) {
        mApi.getRate(inputCurrency, outputCurrency, callback);
    }
}