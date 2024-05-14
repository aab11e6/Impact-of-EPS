### Abstract
The aim of this project is to evaluate the impact of earning report on stock price (by C++). 

Chose 495 S&P 500 component stocks and categorized them according to the spread between their estimated and actual EPS, then calculated accumulated abnormal average returns (CAAR) of each group. Analyzed the impact of the EPS surprise and plotted their graph individually by establishing a user-friendly menu.

### Steps
1. Download all data of S&P 500 from Yahoo Finace by libcurl package.
1. Sample 30 stocks from each of 3 groups randomly.
2. Use Object-Orient Programming idea to creat a class Stock, and use a map to store all the stocks.
3. Prepare a loop that will do this, and the following block, 30 times.
4. Use gunplot to show the result.

### Conclusion

![](https://tva1.sinaimg.cn/large/006tNbRwgy1gb7amvravwj30li0fedj7.jpg)

1. Stocks beating EPS estimates have a better CAAR after EPS was released.

2. Stocks meeting EPS estimates have a gentle CAAR drop after EPS was released

3. Stocks missing EPS estimates have a dramatic CAAR drop after EPS was released

4. Earning report has a significant impact on stock price
