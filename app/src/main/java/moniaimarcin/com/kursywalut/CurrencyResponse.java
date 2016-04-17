package moniaimarcin.com.kursywalut;

import com.google.gson.annotations.SerializedName;

import java.util.ArrayList;
import java.util.Collections;
import java.util.List;
import java.util.Map;

/**
 * Created by lenovo on 2016-04-16.
 */
public class CurrencyResponse {
    @SerializedName("base")
    private String base;
    @SerializedName("date")
    private String date;
    @SerializedName("rates")
    private Map<String, Double> rates;

    public List<String> getAvailableCurrencies(){
        List<String> availableCurrencies = new ArrayList<>();
        for (String currency : rates.keySet()) {
            availableCurrencies.add(currency);
        }
        availableCurrencies.add(base);
        Collections.sort(availableCurrencies);
        return availableCurrencies;
    }

    public Double getRate() {
        return (Double) rates.values().toArray()[0];
    }
    public String getDate() {
        return date;
    }
}
