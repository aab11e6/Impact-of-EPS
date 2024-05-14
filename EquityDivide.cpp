//1. i, iter which one is faster?
//
/*
#include <stdio.h>
#include <vector>
#include <string>
#include <fstream>
#include <cmath>
#include "tools.h"
#include "EquityDivide.h"
using namespace std;

EquityDivide::EquityDivide(const string& path, StockMap& StockList, vector<string>& TickerList)
{
    ifstream file;
    file.open(path);
    
    vector<Ticker> id = read_col<vector<Ticker>, Ticker>(file, 0);
    vector<Date> date = read_col<vector<Date>, Date>(file, 1);
    vector<double> actual = read_col<vector<double>, double>(file, 2);
    vector<double> est = read_col<vector<double>, double>(file, 3);
    
    file.close();
    
    TickerList = id;
    
    vector<string> Tradedate;
    file.open("tradedate.txt");
    string tem;
    while(file >> tem)
    {
        Tradedate.push_back(tem);
    }
    file.close();

    
    
    size_t n = id.size();
    int dateindex;
    for(int i=0;i<n;++i)
    {
        double surprise = (actual[i] - est[i]) / abs(est[i]);
        TickerInfo TI = {id[i], date[i]};
        surprise_vec.push_back({surprise, TI});
        Stock temstock;
        temstock.Ticker = id[i];
        temstock.AnnounceDate = date[i];
        temstock.EstEPS = est[i];
        temstock.ActEPS = actual[i];
        temstock.Surprise = surprise;
    
        for(int j = 30; j <=110 ; j++)
        {
            if(Tradedate[j] == date[i])
            {
                dateindex = j-31;
                j = 110;
            }
        }
        temstock.StartDateIndex = dateindex;
        temstock.StartDate = Tradedate[dateindex];
        temstock.EndDate = Tradedate[dateindex+60];
        
        
        StockList[id[i]] = temstock;
    }
    
    sort(surprise_vec.begin(), surprise_vec.end());
}


Group EquityDivide::divide_group() const
{
    Group ans(3);
    size_t n = surprise_vec.size();
    int group_size = static_cast<int>(n) / 3;
    for(int i=0;i<n;++i){
        //Miss group
        if(i < group_size){
            ans[0].push_back(surprise_vec[i].second);
        }
        //Meet group
        else if(i >= group_size && i < 2 * group_size){
            ans[1].push_back(surprise_vec[i].second);
        }
        //Beat group
        else{
            ans[2].push_back(surprise_vec[i].second);
        }
    }
    return ans;
}
*/

#include <iostream>
#include <stdio.h>
#include <vector>
#include <string>
#include <fstream>
#include <cmath>
#include "tools.h"
#include "EquityDivide.h"
using namespace std;

EquityDivide::EquityDivide(const string& path, StockMap& StockList, vector<string>& TickerList, vector<string>& Tradedate)
{
    ifstream file;
    file.open(path);
    
    vector<Ticker> id = read_col<vector<Ticker>, Ticker>(file, 0);
    vector<Date> date = read_col<vector<Date>, Date>(file, 1);
    vector<double> actual = read_col<vector<double>, double>(file, 2);
    vector<double> est = read_col<vector<double>, double>(file, 3);
    
    file.close();
    
    TickerList = id;
    
    size_t n = id.size();
    int dateindex = 0;
    for(int i=0;i<n;++i)
    {
        double surprise = (actual[i] - est[i]) / abs(est[i]);
        TickerInfo TI = {id[i], date[i]};
        surprise_vec.push_back({surprise, TI});
        Stock temstock;
        temstock.Ticker = id[i];
        temstock.AnnounceDate = date[i];
        temstock.EstEPS = est[i];
        temstock.ActEPS = actual[i];
        temstock.Surprise = surprise;
        
        cout << date[i] << endl;
        
        for(int j = 30; j <=124 ; j++)
        {
            if(Tradedate[j] == date[i])
            {
                dateindex = j-31;
                j = 124;
            }
        }
        temstock.StartDateIndex = dateindex;
        temstock.StartDate = Tradedate[dateindex];
        temstock.EndDate = Tradedate[dateindex+60];
        
        
        StockList[id[i]] = temstock;
    }
    
    sort(surprise_vec.begin(), surprise_vec.end());
}


Group EquityDivide::divide_group() const
{
    Group ans(3);
    size_t n = surprise_vec.size();
    int group_size = static_cast<int>(n) / 3;
    for(int i=0;i<n;++i){
        //Miss group
        if(i < group_size){
            ans[0].push_back(surprise_vec[i].second);
        }
        //Meet group
        else if(i >= group_size && i < 2 * group_size){
            ans[1].push_back(surprise_vec[i].second);
        }
        //Beat group
        else{
            ans[2].push_back(surprise_vec[i].second);
        }
    }
    return ans;
}
