
#include <stdio.h>
#include <string>
#include <iostream>
#include <sstream>
#include <vector>
#include <locale>
#include <iomanip>
#include "curl/curl.h"
#include "Stock.h"

typedef map<string,Stock> StockMap;

using namespace std;

size_t write_data(void *ptr, int size, int nmemb, FILE *stream)
{
    size_t written;
    written = fwrite(ptr, size, nmemb, stream);
    return written;
}

struct MemoryStruct {
    char *memory;
    size_t size;
};

void *myrealloc(void *ptr, size_t size)
{
    /* There might be a realloc() out there that doesn't like reallocing
     NULL pointers, so we take care of it here */
    if (ptr)
        return realloc(ptr, size);
    else
        return malloc(size);
}


size_t write_data2(void *ptr, size_t size, size_t nmemb, void *data)
{
    size_t realsize = size * nmemb;
    struct MemoryStruct *mem = (struct MemoryStruct *)data;
    
    mem->memory = (char *)myrealloc(mem->memory, mem->size + realsize + 1);
    if (mem->memory) {
        memcpy(&(mem->memory[mem->size]), ptr, realsize);
        mem->size += realsize;
        mem->memory[mem->size] = 0;
    }
    return realsize;
}

string getTimeinSeconds(string Time)
{
    std::tm t = {0};
    std::istringstream ssTime(Time);
    char time[100];
    memset(time, 0, 100);
    if (ssTime >> std::get_time(&t, "%Y-%m-%dT%H:%M:%S"))
    {
        cout << std::put_time(&t, "%c %Z") << "\n"
        << std::mktime(&t) << "\n";
        sprintf (time, "%ld", mktime(&t));
        return string(time);
    }
    else
    {
        cout << "Parse failed\n";
        return "";
    }
}

int Extract(StockMap &StockList)
{
    auto itr = StockList.begin();
    
    struct MemoryStruct data;
    data.memory = NULL;
    data.size = 0;
    
    //file pointer to create file that store the data
    FILE *fp;
    
    /* declaration of an object CURL */
    CURL *handle;
    
    /* result of the whole process */
    CURLcode result;
    
    /* the first functions */
    /* set up the program environment that libcurl needs */
    curl_global_init(CURL_GLOBAL_ALL);
    
    /* curl_easy_init() returns a CURL easy handle that you're gonna reuse in other easy functions*/
    handle = curl_easy_init();
    
    /* if everything's all right with the easy handle... */
    if (handle)
    {
        string sCookies, sCrumb;
        if (sCookies.length() == 0 || sCrumb.length() == 0)
        {
            curl_easy_setopt(handle, CURLOPT_URL, "https://finance.yahoo.com/quote/AMZN/history");
            
            curl_easy_setopt(handle, CURLOPT_SSL_VERIFYPEER, 0);
            curl_easy_setopt(handle, CURLOPT_SSL_VERIFYHOST, 0);
            curl_easy_setopt(handle, CURLOPT_COOKIEJAR, "cookies.txt");
            curl_easy_setopt(handle, CURLOPT_COOKIEFILE, "cookies.txt");
            
            curl_easy_setopt(handle, CURLOPT_ENCODING, "");
            curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, write_data2);
            curl_easy_setopt(handle, CURLOPT_WRITEDATA, (void *)&data);
            result = curl_easy_perform(handle);
            
            
            /* perform, then store the expected code in 'result'*/
            result = curl_easy_perform(handle);
            
            /* Check for errors */
            if (result != CURLE_OK)
            {
                /* if errors have occured, tell us what is wrong with 'result'*/
                fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(result));
                return 1;
            }
            
            char cKey[] = "CrumbStore\":{\"crumb\":\"";
            char *ptr1 = strstr(data.memory, cKey);
            char *ptr2 = ptr1 + strlen(cKey);
            char *ptr3 = strstr(ptr2, "\"}");
            if ( ptr3 != NULL )
                *ptr3 = NULL;
            
            sCrumb = ptr2;
            
            fp = fopen("cookies.txt", "r");
            char cCookies[100];
            if (fp) {
                while (fscanf(fp, "%s", cCookies) != EOF);
                fclose(fp);
            }
            else
                cerr << "cookies.txt does not exists" << endl;
            
            sCookies = cCookies;
        }
        while (true)
        {
            
            string startTime = getTimeinSeconds(itr->second.StartDate+"T16:00:00");
            string endTime = getTimeinSeconds(itr->second.EndDate+"T16:00:00");
            
            free(data.memory);
            data.memory = NULL;
            data.size= 0;
            
            
            string urlA = "https://query1.finance.yahoo.com/v7/finance/download/";
            string symbol = itr->first;
            string urlB = "?period1=";
            string urlC = "&period2=";
            string urlD = "&interval=1d&events=history&crumb=";
            string url = urlA + symbol + urlB + startTime + urlC + endTime + urlD + sCrumb;
            const char * cURL = url.c_str();
            const char * cookies = sCookies.c_str();
            curl_easy_setopt(handle, CURLOPT_COOKIE, cookies);   // Only needed for 1st stock
            curl_easy_setopt(handle, CURLOPT_URL, cURL);
            curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, write_data2);
            curl_easy_setopt(handle, CURLOPT_WRITEDATA, (void *)&data);
            result = curl_easy_perform(handle);
            
            /* Check for errors */
            if (result != CURLE_OK)
            {
                /* if errors have occurred, tell us what is wrong with 'result'*/
                fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(result));
                return 1;
            }
            stringstream sData;
            sData.str(data.memory);
            string adjprice;
            vector<string> tempvector;
            string line;
            getline(sData, line);
            //cout << itr->first << endl; // cout the ticker to show the process of retrieving data
            while (getline(sData, line)) {
                line.erase(line.find_last_of(','));
                adjprice = line.substr(line.find_last_of(',') + 1);
                tempvector.push_back(adjprice);
            }
            itr->second.AdjClose = tempvector;
            if(itr->second.AdjClose.size() != 61)
            {
                cout << "NO ENOUGH DATA!! WRONG!!" << endl;
                cout << itr->first << " " << itr->second.AdjClose.size() << endl;
            }
            itr++;
            
            if (itr == StockList.end())
                break;
        }
        free(data.memory);
        data.size= 0;
    }
    else
    {
        fprintf(stderr, "Curl init failed!\n");
        return 1;
    }
    
    /* cleanup since you've used curl_easy_init */
    curl_easy_cleanup(handle);
    
    /* this function releases resources acquired by curl_global_init() */
    curl_global_cleanup();
    return 0;
}


int ExtractSPY(vector<string>& SPY, vector<string>& DATE, string StartDate, string EndDate)
{
    string startTime = getTimeinSeconds(StartDate+"T16:00:00");
    string endTime = getTimeinSeconds(EndDate+"T16:00:00");
    
    struct MemoryStruct data;
    data.memory = NULL;
    data.size = 0;
    
    //file pointer to create file that store the data
    FILE *fp;
    
    /* declaration of an object CURL */
    CURL *handle;
    
    /* result of the whole process */
    CURLcode result;
    
    /* the first functions */
    /* set up the program environment that libcurl needs */
    curl_global_init(CURL_GLOBAL_ALL);
    
    /* curl_easy_init() returns a CURL easy handle that you're gonna reuse in other easy functions*/
    handle = curl_easy_init();
    
    /* if everything's all right with the easy handle... */
    if (handle)
    {
        string sCookies, sCrumb;
        if (sCookies.length() == 0 || sCrumb.length() == 0)
        {
            curl_easy_setopt(handle, CURLOPT_URL, "https://finance.yahoo.com/quote/AMZN/history");
            
            curl_easy_setopt(handle, CURLOPT_SSL_VERIFYPEER, 0);
            curl_easy_setopt(handle, CURLOPT_SSL_VERIFYHOST, 0);
            curl_easy_setopt(handle, CURLOPT_COOKIEJAR, "cookies.txt");
            curl_easy_setopt(handle, CURLOPT_COOKIEFILE, "cookies.txt");
            
            curl_easy_setopt(handle, CURLOPT_ENCODING, "");
            curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, write_data2);
            curl_easy_setopt(handle, CURLOPT_WRITEDATA, (void *)&data);
            result = curl_easy_perform(handle);
            
            /* perform, then store the expected code in 'result'*/
            result = curl_easy_perform(handle);
            
            /* Check for errors */
            if (result != CURLE_OK)
            {
                /* if errors have occured, tell us what is wrong with 'result'*/
                fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(result));
                return 1;
            }
            
            char cKey[] = "CrumbStore\":{\"crumb\":\"";
            char *ptr1 = strstr(data.memory, cKey);
            char *ptr2 = ptr1 + strlen(cKey);
            char *ptr3 = strstr(ptr2, "\"}");
            if ( ptr3 != NULL )
                *ptr3 = NULL;
            
            sCrumb = ptr2;
            
            fp = fopen("cookies.txt", "r");
            char cCookies[100];
            if (fp) {
                while (fscanf(fp, "%s", cCookies) != EOF);
                fclose(fp);
            }
            else
                cerr << "cookies.txt does not exists" << endl;
            
            sCookies = cCookies;
        }
        
        free(data.memory);
        data.memory = NULL;
        data.size= 0;
        
        string urlA = "https://query1.finance.yahoo.com/v7/finance/download/";
        string symbol = "^GSPC";
        string urlB = "?period1=";
        string urlC = "&period2=";
        string urlD = "&interval=1d&events=history&crumb=";
        string url = urlA + symbol + urlB + startTime + urlC + endTime + urlD + sCrumb;
        const char * cURL = url.c_str();
        const char * cookies = sCookies.c_str();
        curl_easy_setopt(handle, CURLOPT_COOKIE, cookies);   // Only needed for 1st stock
        curl_easy_setopt(handle, CURLOPT_URL, cURL);
        curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, write_data2);
        curl_easy_setopt(handle, CURLOPT_WRITEDATA, (void *)&data);
        result = curl_easy_perform(handle);
        
        /* Check for errors */
        if (result != CURLE_OK)
        {
            /* if errors have occurred, tell us what is wrong with 'result'*/
            fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(result));
            return 1;
        }
        
        stringstream sData;
        sData.str(data.memory);
        string adjprice,date;
        vector<string> tempvector;
        string line;
        getline(sData, line);
        while (getline(sData, line)) {
            cout << line << endl;
            line.erase(line.find_last_of(','));
            date = line.substr(0,10);
            adjprice = line.substr(line.find_last_of(',') + 1);
            SPY.push_back(adjprice);
            DATE.push_back(date);
        }
        cout << "SPY data, done!" << endl;
        
        free(data.memory);
        data.size= 0;
    }
    else
    {
        fprintf(stderr, "Curl init failed!\n");
        return 1;
    }
    
    /* cleanup since you've used curl_easy_init */
    curl_easy_cleanup(handle);
    
    /* this function releases resources acquired by curl_global_init() */
    curl_global_cleanup();
    return 1;
}
