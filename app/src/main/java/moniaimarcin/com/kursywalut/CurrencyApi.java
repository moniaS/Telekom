package moniaimarcin.com.kursywalut;

import retrofit.Callback;
import retrofit.http.GET;
import retrofit.http.Query;

/**
 * Created by Marcin Szalek on 2016-02-29.
 */
public interface CurrencyApi {

    @GET("/latest")
    void getAvailableCurrencies(
            Callback<CurrencyResponse> callback
    );

    @GET("/latest")
    void getRate(
            @Query("base")String baseCurrency,
            @Query("symbols")String targetCurrency,
            Callback<CurrencyResponse> callback
    );

}

