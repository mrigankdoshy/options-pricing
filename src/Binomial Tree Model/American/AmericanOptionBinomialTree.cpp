/* 
Assumptions in a binomial option pricing model: 
	1. Only two possible prices, one up and one down
	2. The underlying asset pays no dividends
	3. Interest rate is constant
	4. No taxes and transaction costs.

	Option value = [(probability of rise * up value) + (probability of drop * down value)] / (1+r) 

Valuation is performed iteratively, starting at each of the final nodes 
(those that may be reached at the time of expiration), and then working 
backwards through the tree towards the first node (valuation date). The value 
computed at each stage is the value of the option at that point in time.

Option valuation using this method is, as described, a three-step process:
	1. Price tree generation,
	2. Calculation of option value at each final node,
	3. Sequential calculation of the option value at each preceding node.
*/
#include <iostream>
#include <math.h>
#include <vector>

using namespace std;

vector <int> Nodes(int n)
{
	vector <int> nodes;

	nodes.push_back(n * 4 + 2);
	nodes.push_back(n + 1);

	return nodes;
}

/*
STEP 1:
The up and down factors are calculated using the underlying volatility (sigma) 
and the time duration of a step (t), measured in years. From the condition that 
the variance of the log of the price is sigma^2*t, we have:
	1. u = e^{sigma*sqrt(t)}
	2. d=e^{-sigma*sqrt(t)}

*/
vector <double> UpDownFactor(double volatility, double maturity, int nodes)
{
	/* Calculate t in years and compute u and d using the formulas above */
	double dt = maturity / (double) nodes;
	vector <double> result;

	result.push_back(exp(volatility * sqrt(dt)));
	result.push_back(exp(-volatility * sqrt(dt)));
	
	return result;
}

/*
STEP 2: 
At each final node of the tree i.e. at expiration of the option, the option 
value is simply its intrinsic, or exercise, value.
	1. Max [ (S − K), 0 ], for a call option
	2. Max [ (K − S), 0 ], for a put option:
Where K is the strike price and S is the spot price of the underlying asset at 
the nth period.
*/
double OptionPremium(double S, double K, int optionType)
{
	double result = 0;

	/* For Call Option*/
	if(optionType == 0) {
		result = S - K;
	}
	/* For Put Option */
	else {
		result = K - S;
	}
	if(result < 0) {
		return 0;
	}
	else {
		return result;
	}
}

/*
STEP 3:
Once the above step is complete, the option value is then found for each node, 
starting at the penultimate time step, and working back to the first node of 
the tree (the valuation date) where the calculated result is the value of the 
option. In overview: the "binomial value" is found at each node, using the risk 
neutrality assumption. If exercise is permitted at the node, then the model 
takes the greater of binomial and exercise value at the node.
*/
vector <double> ProbabilityUpDown(vector <double> UpAndDown, 
	double riskFreeRate, double maturity, int nodes)
{
	double probabilityUp, probabilityDown;
	double dt = maturity / (double) nodes; //dt = t/n
	
	vector <double> result;
	/*
		 	 r(dt)
	    	e      - d
	  pUp = -----------
		   	  u - d
	*/
	probabilityUp = (exp(riskFreeRate * dt) - UpAndDown[1]) / (UpAndDown[0] - UpAndDown[1]);
	probabilityDown = 1 - probabilityUp;
	result.push_back(probabilityUp);
	result.push_back(probabilityDown);
	
	return result;
}

double Discount(double A, double B, double Up, double Down)
{
	return Up * A + Down * B;
}

void PrintTree(double ** Matrix, int m, int n)
{
	for(int i = 0; i < m; ++i) {
		for(int j = 0; j < n; ++j) {
			if(Matrix[i][j] == 0) {
				cout << "" << "\t";
			}
			else {
				cout << Matrix[i][j] << "\t";
			}
		}
		cout << endl;
	}
}

double ** BuildDoubleArray(int m, int n)
{
	double ** x = new double*[m];
	for(int i = 0; i < m; ++i) {
		x[i] = new double[n];
		for(int j = 0; j < n; ++j) {
			x[i][j] = 0;
		}
	}
	return x;
}

/* Values with zeros / nothing below it*/
double ** ForwardTree(double ** data, double S, double pUp, double pDown, int x, 
	int y)
{
	int start = floor(x / 2) - 1;
	int loop = 0;
	
	if((y - 1) % 2 == 0) {
		loop = floor(y / 2) + 1;
	}
	else {
		loop = floor(y / 2);
	}

	for(int j = 0; j < loop; ++j) {
		data[start][2*j] = S;
		for(int i = 1; i < y - (2*j); ++i) { 
			data[start - (2*i)][i + (2*j)] = data[start - (2 *(i-1))][(i - 1) + (2*j)] * pUp;
			data[start + (2*i)][i + (2*j)] = data[start + (2 *(i-1))][(i - 1) + (2*j)] * pDown;	
		}
	}
	
	return data;
}

/* Premium in the last column */
double ** CalculatePremium(double ** data, double K, int optionType, int y)
{
	for(int i = 0; i < y; ++i) {
		data[4 * i + 1][y - 1] = OptionPremium(data[4 * i][y - 1], K, 
			optionType); 
	}

	return data;
}

double ** BackwardTree(double ** data, double K, double pUp, double pDown, 
	int optionType, int y)
{
	double up, down, discount, premium;
	
	for(int j = 0; j < y; ++j) {
		for(int i = 0; i < y - (1 + j); ++i) {
			up = data[(2*j) + 4 * i + 1][y - (1 + j)]; 
			down = data[(2*j) + 4 * (i + 1) + 1][y - (1 + j)];
			discount = Discount(up, down, pUp, pDown);
			premium = OptionPremium(data[(2*(j+1)) + 4 * i][y - (2 + j)], K, 
				optionType);
			if(discount > premium) {
				data[(2*(j+1)) + 4 * i + 1][y - (2 + j)] = discount;
			}
			else {
				data[(2*(j+1)) + 4 * i + 1][y - (2 + j)] = premium;
			}
		}
	}
	return data;
}

double OptionPrice(double ** data, int x)
{
	int start = floor(x / 2);
	return data[start][0];
}

int main()
{

	double stockPrice, strikePrice, riskFreeRate, volatility, maturity;
	
	int optionType, nodes, defaultChoice, displayTree;
	
	cout << "\nBinomial Option Pricer\n------------------------------------\n";
	
	cout << "Use default values given below?"<<endl;
	cout << "Stock Price = 100"<<endl 
		<<"Strike Price = 95"<<endl
		<<"Risk Free Rate = 5.87"<<endl
		<<"Volatility = 53.66"<<endl
		<<"Maturity = 5"<<endl
		<<"Option Type = Call"<<endl
		<<"Nodes = 5"<<endl
		<<"Display Tree = Yes"<<endl 
		<<"('1' for Yes, '0' for No): "<<endl;
	cin >> defaultChoice;
	
	if(defaultChoice == 1) {
		stockPrice = 100;
		strikePrice = 95;
		riskFreeRate = 5;
		volatility = 50;
		maturity = 5;
		optionType = 1;
		nodes = 5;
		displayTree = 1;
	} 
	else {
		cout << "Enter your stock price: ";
		cin >> stockPrice;
		cout << "Enter your strike price: ";
		cin >> strikePrice;
		cout << "Enter your risk-free rate: ";
		cin >> riskFreeRate;
		cout << "Enter your volatility: ";
		cin >> volatility;
		cout << "Enter your maturity in months: ";
		cin >> maturity;	
		cout << "Enter Option Type ('1' for Put, '0' for Call): ";
		cin >> optionType;
		cout << "Enter the number of nodes: ";
		cin >> nodes;
		cout << "Display Tree? ('1' for Yes, '0' for No): ";
		cin >> displayTree;
	}
	
	riskFreeRate /= 100;
	volatility /= 100;
	maturity /= 12;

	vector <int> xy = Nodes(nodes);
	vector <double> upDownFactor = UpDownFactor(volatility, maturity, nodes);
	vector <double> probabilityUpDown = ProbabilityUpDown(upDownFactor, 
		riskFreeRate, maturity, nodes);
	
	int x = xy[0];
	int y = xy[1];

	double up = upDownFactor[0];
	double down = upDownFactor[1];
	double probabilityUp = probabilityUpDown[0];
	double probabilityDown = probabilityUpDown[1];
	
	double ** data = BuildDoubleArray(x, y);
	
	data = ForwardTree(data, stockPrice, up, down, x, y);
	data = CalculatePremium(data, strikePrice, optionType, y);
	data = BackwardTree(data, strikePrice, probabilityUp, probabilityDown, 
		optionType, y);
	
	double optionPrice = OptionPrice(data, x);
	
	if(displayTree == 1) {
		PrintTree(data, x, y);
	}
	
	cout << endl;
	cout << "Option Price: " << optionPrice << endl;
	
	return 0;
}
