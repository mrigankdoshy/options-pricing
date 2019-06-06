#include <iostream>
#include <math.h>
#include <vector>

std::vector <int> Nodes(int n)
{
	std::vector <int> nodes;
	nodes.push_back(n * 4 + 2);
	nodes.push_back(n + 1);
	
	return nodes;
}

std::vector <double> UpDownFactor(double volatility, double maturity, int nodes)
{
	double dt = maturity / (double) nodes;
	
	std::vector <double> result;
	result.push_back(exp(volatility * sqrt(dt)));
	result.push_back(exp(-volatility * sqrt(dt)));
	
	return result;
}

std::vector <double> ProbabilityUpDown(std::vector <double> UpAndDown, double riskFree, double maturity, int nodes)
{
	double p_up, p_down;
	double dt = maturity / (double) nodes;
	
	std::vector <double> result;
	
	p_up = exp(riskFree * dt) - UpAndDown[1];
	p_up /= (UpAndDown[0] - UpAndDown[1]);
	p_down = 1 - p_up;
	
	result.push_back(p_up);
	result.push_back(p_down);
	
	return result;
}

double OptionPremium(double S, double K, int opType)
{
	double result = 0;
	if(opType == 0){
		result = S - K;
	} else {
		result = K - S;
	}
	if(result < 0){
		return 0;
	} else {
		return result;
	}
}

double Discount(double A, double B, double Up, double Down)
{
	return Up * A + Down * B;
}


void PrintTree(double ** Matrix, int m, int n)
{
	for(int i = 0; i < m; ++i){
		for(int j = 0; j < n; ++j){
			if(Matrix[i][j] == 0){
				std::cout << "" << "\t";
			} else {
				std::cout << Matrix[i][j] << "\t";
			}
		}
		std::cout << std::endl;
	}
}


double ** BuildDoubleArray(int m, int n)
{
	double ** x = new double*[m];
	for(int i = 0; i < m; ++i){
		x[i] = new double[n];
		for(int j = 0; j < n; ++j)
		{
			x[i][j] = 0;
		}
	}
	return x;
}


double ** ForwardTree(double ** data, double S, double pUp, double pDown, int x, int y)
{
	
	int start = floor(x / 2) - 1;
	int loop = 0;
	
	if((y - 1) % 2 == 0){
		loop = floor(y / 2) + 1;
	} else {
		loop = floor(y / 2);
	}

	for(int j = 0; j < loop; ++j)
	{
		data[start][2*j] = S;
		for(int i = 1; i < y - (2*j); ++i)
		{
			data[start - (2*i)][i + (2*j)] = data[start - (2 *(i-1))][(i - 1) + (2*j)] * pUp;
			data[start + (2*i)][i + (2*j)] = data[start + (2 *(i-1))][(i - 1) + (2*j)] * pDown;		
		}
	}
	
	return data;
}

double ** CalculatePremium(double ** data, double K, int opType, int y)
{
	for(int i = 0; i < y; ++i)
	{
		data[4 * i + 1][y - 1] = OptionPremium(data[4 * i][y - 1], K, opType);	
	}
	return data;
}

double ** BackwardTree(double ** data, double K, double pUp, double pDown, int opType, int y)
{
	double up, down, discount, premium;
	
	for(int j = 0; j < y; ++j)
	{
		for(int i = 0; i < y - (1 + j); ++i)
		{
			up = data[(2*j) + 4 * i + 1][y - (1 + j)];
			down = data[(2*j) + 4 * (i + 1) + 1][y - (1 + j)];
			discount = Discount(up, down, pUp, pDown);
			premium = OptionPremium(data[(2*(j+1)) + 4 * i][y - (2 + j)], K, opType);
			if(discount > premium){
				data[(2*(j+1)) + 4 * i + 1][y - (2 + j)] = discount;
			} else {
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

	double stock_price, strike_price, risk_free, volatility, maturity;
	
	int opType, nodes, default_choice, show_tree;
	
	std::cout << "\nBinomial Option Pricer\n------------------------------------\n";
	
	std::cout << "Default (0 = Yes 1 = No): ";
	std::cin >> default_choice;
	
	if(default_choice == 0)
	{
		stock_price = 100;
		strike_price = 95;
		risk_free = 4.66;
		volatility = 55.44;
		maturity = 7;
		opType = 1;
		nodes = 7;
		show_tree = 0;
	} 
	else 
	{
		std::cout << "Enter your stock price: ";
		std::cin >> stock_price;
		std::cout << "Enter your strike price: ";
		std::cin >> strike_price;
		std::cout << "Enter your risk-free rate: ";
		std::cin >> risk_free;
		std::cout << "Enter your volatility: ";
		std::cin >> volatility;
		std::cout << "Enter your maturity in months: ";
		std::cin >> maturity;	
		std::cout << "Enter Option Type (0 = Call 1 = Put): ";
		std::cin >> opType;
		std::cout << "Enter the number of nodes: ";
		std::cin >> nodes;
		std::cout << "Show Tree (0 = Yes 1 = No): ";
		std::cin >> show_tree;
	}
	
	risk_free /= 100;
	volatility /= 100;
	maturity /= 12;

	std::vector <int> xy = Nodes(nodes);
	std::vector <double> updown = UpDownFactor(volatility, maturity, nodes);
	std::vector <double> probUD = ProbabilityUpDown(updown, risk_free, maturity, nodes);
	
	int x = xy[0];
	int y = xy[1];
	double up = updown[0];
	double down = updown[1];
	double p_up = probUD[0];
	double p_down = probUD[1];
	
	double ** data = BuildDoubleArray(x, y);
	
	data = ForwardTree(data, stock_price, up, down, x, y);
	data = CalculatePremium(data, strike_price, opType, y);
	data = BackwardTree(data, strike_price, p_up, p_down, opType, y);
	
	double option_price = OptionPrice(data, x);
	
	if(show_tree == 0){
		PrintTree(data, x, y);
	}
	
	std::cout << std::endl;
	std::cout << "Option Price: " << option_price << std::endl;
	
	return 0;
}
