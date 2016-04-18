package moniaimarcin.com.kursywalut;

import android.app.Activity;
import android.app.AlertDialog;
import android.content.Context;
import android.os.Bundle;
import android.support.v7.app.AppCompatActivity;
import android.support.v7.widget.Toolbar;
import android.text.Editable;
import android.text.TextWatcher;
import android.util.Log;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.view.ViewGroup;
import android.view.inputmethod.InputMethodManager;
import android.widget.AdapterView;
import android.widget.ArrayAdapter;
import android.widget.EditText;
import android.widget.Spinner;
import android.widget.TextView;


import java.util.ArrayList;
import java.util.List;

import retrofit.Callback;
import retrofit.RetrofitError;
import retrofit.client.Response;

public class MainActivity extends AppCompatActivity implements View.OnClickListener {

    private List<String> availableCurrencies = new ArrayList<>();
    private EditText etInput;
    private TextView tvOutput;
    private Spinner inputSpinner;
    private Spinner outputSpinner;
    private View loadingPanel;
    private final String TAG = getClass().getSimpleName();

    private Callback<CurrencyResponse> mGetCurrenciesCallback = new Callback<CurrencyResponse>() {
        @Override
        public void success(CurrencyResponse currencyResponse, Response response) {
            loadingPanel.setVisibility(View.GONE);
            Log.i(TAG, "mamy waluty ;)");
            availableCurrencies = currencyResponse.getAvailableCurrencies();
            setSpinnersContent();
        }

        @Override
        public void failure(RetrofitError error) {
            loadingPanel.setVisibility(View.GONE);
            Log.e(TAG, "Error:", error.getCause());
            availableCurrencies = new ArrayList<>();
            String lastInputCurrency = SharedPreferencesManager.getLastInputCurrency(MainActivity.this);
            String lastOutputCurrency = SharedPreferencesManager.getLastOutputCurrency(MainActivity.this);
            availableCurrencies.add(lastInputCurrency);
            availableCurrencies.add(lastOutputCurrency);
            setSpinnersContent();
            showAlertDialog();
        }
    };

    private Callback<CurrencyResponse> mGetRateCallback = new Callback<CurrencyResponse>() {
        @Override
        public void success(CurrencyResponse currencyResponse, Response response) {
            loadingPanel.setVisibility(View.GONE);
            Log.i(TAG, "mamy kurs ;)");
            updateSharedPreferences(currencyResponse.getRate(), currencyResponse.getDate());
            calculateAndDisplay();
            displayLastDate();
        }

        @Override
        public void failure(RetrofitError error) {
            loadingPanel.setVisibility(View.GONE);
            Log.e(TAG, "Error:", error.getCause());
            showAlertDialog();
            inputSpinner.setSelection(getCurrencyPostion(SharedPreferencesManager.getLastInputCurrency(MainActivity.this)));
            outputSpinner.setSelection(getCurrencyPostion(SharedPreferencesManager.getLastOutputCurrency(MainActivity.this)));
        }
    };

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        Toolbar toolbar = (Toolbar) findViewById(R.id.toolbar);
        setSupportActionBar(toolbar);

        initViews();
        displayLastDate();
        sendCurrenciesRequest();
    }

    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        getMenuInflater().inflate(R.menu.main_menu, menu);
        return true;
    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        switch (item.getItemId()) {
            case R.id.refreshCurrencies:
                sendCurrenciesRequest();
                break;
            case R.id.refreshRate:
                sendRateRequest();
                break;
        }

        return true;
    }

    private void displayLastDate() {
        ((TextView) findViewById(R.id.tvLastDate)).setText("Kurs waluty z dnia " + SharedPreferencesManager.getLastRateDate(MainActivity.this));
    }

    private void showAlertDialog() {
        AlertDialog.Builder builder = new AlertDialog.Builder(this);
        builder.setTitle("Błąd");
        builder.setMessage("Brak połączenia z internetem. \nDziałasz na ostatnio pobranych danych.\nW menu masz opcję synchronizacji.");
        builder.setPositiveButton("OK", null);
        builder.create().show();
    }

    private void initViews() {
        initSpinners();
        findViewById(R.id.main_layout).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                InputMethodManager imm = (InputMethodManager) getSystemService(Activity.INPUT_METHOD_SERVICE);
                imm.hideSoftInputFromWindow(getCurrentFocus().getWindowToken(), 0);
            }
        });
        loadingPanel = findViewById(R.id.loadingPanel);
        tvOutput = (TextView) findViewById(R.id.tvOutput);
        etInput = (EditText) findViewById(R.id.etInput);
        etInput.addTextChangedListener(new TextWatcher() {
            @Override
            public void beforeTextChanged(CharSequence s, int start, int count, int after) {
            }

            @Override
            public void onTextChanged(CharSequence s, int start, int before, int count) {
            }

            @Override
            public void afterTextChanged(Editable s) {
                calculateAndDisplay();
            }
        });
    }

    private void setSpinnersContent() {
        CurrencySpinnerAdapter inputSpinnerAdapter = new CurrencySpinnerAdapter(MainActivity.this, availableCurrencies);
        inputSpinner.setAdapter(inputSpinnerAdapter);
        inputSpinner.setSelection(getCurrencyPostion(SharedPreferencesManager.getLastInputCurrency(MainActivity.this)));

        CurrencySpinnerAdapter outputSpinnerAdapter = new CurrencySpinnerAdapter(MainActivity.this, availableCurrencies);
        outputSpinner.setAdapter(outputSpinnerAdapter);
        outputSpinner.setSelection(getCurrencyPostion(SharedPreferencesManager.getLastOutputCurrency(MainActivity.this)));
    }

    private void initSpinners() {
        inputSpinner = (Spinner) findViewById(R.id.inputCurrencySpinner);
        outputSpinner = (Spinner) findViewById(R.id.outputCurrencySpinner);
        inputSpinner.setOnItemSelectedListener(new OnCurrencySelectedListener(this, true));
        outputSpinner.setOnItemSelectedListener(new OnCurrencySelectedListener(this, false));
    }

    private void calculateAndDisplay() {
        String input = etInput.getText().toString();
        double inputValue = input.isEmpty() ? 0 : Double.parseDouble(input);
        Double outputValue = inputValue * SharedPreferencesManager.getLastRate(this);
        tvOutput.setText(String.format("%.2f", outputValue));
    }

    private void sendRateRequest() {
        loadingPanel.setVisibility(View.VISIBLE);
        new CurrencyRestAdapter().getRate((String) inputSpinner.getSelectedItem(), (String) outputSpinner.getSelectedItem(), mGetRateCallback);
    }

    private void sendCurrenciesRequest() {
        loadingPanel.setVisibility(View.VISIBLE);
        new CurrencyRestAdapter().getAvailableCurrencies(mGetCurrenciesCallback);
    }

    private void updateSharedPreferences(double rate, String date) {
        updateSharedPreferences(rate);
        SharedPreferencesManager.saveLastRateDate(this, date);
    }

    private void updateSharedPreferences(double rate) {
        saveLastCurrencies((String) inputSpinner.getSelectedItem(), (String) outputSpinner.getSelectedItem());
        SharedPreferencesManager.saveLastRate(this, rate);
    }

    private void saveLastCurrencies(String inputCurrency, String outputCurrency) {
        SharedPreferencesManager.saveLastInputCurrency(MainActivity.this, inputCurrency);
        SharedPreferencesManager.saveLastOutputCurrency(MainActivity.this, outputCurrency);
    }

    private int getCurrencyPostion(String targetCurrency) {
        for (int i = 0; i < availableCurrencies.size(); i++) {
            if (availableCurrencies.get(i).equals(targetCurrency))
                return i;
        }
        return 0;
    }

    @Override
    public void onClick(View v) {
        View view = this.getCurrentFocus();
        if (view != null) {
            InputMethodManager imm = (InputMethodManager) getSystemService(Activity.INPUT_METHOD_SERVICE);
            imm.hideSoftInputFromWindow(view.getWindowToken(), 0);
        }
    }

    private class OnCurrencySelectedListener implements AdapterView.OnItemSelectedListener {
        private boolean isInputSpinner;
        private Context context;

        public OnCurrencySelectedListener(Context context, boolean isInputSpinner) {
            this.isInputSpinner = isInputSpinner;
            this.context = context;
        }

        @Override
        public void onItemSelected(AdapterView<?> parent, View view, int position, long id) {
            boolean switchOutputSpinner = isInputSpinner && (inputSpinner.getSelectedItem()).equals(SharedPreferencesManager.getLastOutputCurrency(context));
            if (switchOutputSpinner) {
                outputSpinner.setSelection(getCurrencyPostion(SharedPreferencesManager.getLastInputCurrency(context)));
            } else if (!isInputSpinner && (outputSpinner.getSelectedItem()).equals(SharedPreferencesManager.getLastInputCurrency(context))) {
                inputSpinner.setSelection(getCurrencyPostion(SharedPreferencesManager.getLastOutputCurrency(context)));
            }
            if (availableCurrencies.size() > 2) {
                sendRateRequest();
            } else if (isInputSpinner) {
                //TU JEST TAKI MYK, bo chodzi o to ze ta funkcja przy zamianie tej samej waluty wywlouje sie dwa razy,
                // a nie chcemy zeby sie wywyolywala 2 razy wiec dajemy tu ifa #notMyCode
                updateSharedPreferences(1 / SharedPreferencesManager.getLastRate(context));
                calculateAndDisplay();
            }

        }

        @Override
        public void onNothingSelected(AdapterView<?> parent) {
            Log.i("asd", "nothing selected");
        }
    }

    private class CurrencySpinnerAdapter extends ArrayAdapter<String> {
        public CurrencySpinnerAdapter(Context context, List<String> currencies) {
            super(context, android.R.layout.simple_spinner_item, currencies);
        }

        @Override
        public View getView(int position, View convertView, ViewGroup parent) {
            TextView tvCurrency = (TextView) super.getView(position, convertView, parent);
            tvCurrency.setText(getItem(position));
            return tvCurrency;
        }

        @Override
        public View getDropDownView(int position, View convertView, ViewGroup parent) {
            return getView(position, convertView, parent);
        }
    }

}
