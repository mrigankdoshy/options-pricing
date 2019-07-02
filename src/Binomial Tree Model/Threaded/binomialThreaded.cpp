#include <iostream>
#include <vector>
#include <math.h>
#include <thread>
#include <mutex>
#include <string>
#include <time.h>

std::mutex mtx;

auto cc = [](int n, int i){return n - i;};
auto min = [](double a, double b){if(a > b){return a;} else {return b;}};

double nonce()
{
	time_t time1;
	time(&time1);
	return time1;
}

double Prob(double r, double t, double n, double P, double D)
{
	double result = (exp(r*(t/n)) - D) / (P - D);
	return result;
}

double Jump(double v, double t, double n, bool choice)
{
	double result;
	if(choice == true)
	{
		result = exp(v*-sqrt(t/n));
	} else {
		result = exp(v*sqrt(t/n));
	}
	return result;
}

void Spread(double * &temp, double S, double K, double P, double D, int n, int a, int b, std::string opType)
{
	//mtx.lock();
	int ii;	
	double Q;
	
	for(int i = a; i < b; ++i)
	{
		ii = cc(n, i);
		Q = S * pow(P, ii) * pow(D, i);
		
		if(opType == "Call")
		{
			temp[i] = min(Q - K, 0);
		} else {
			temp[i] = min(K - Q, 0);
		}
		
	}
	//mtx.unlock();
}

void Discounter(double * &first, double A, double B, double t, double n, int a, int b)
{
	//mtx.lock();
	for(unsigned i = a; i < b; ++i)
	{
		first[i] = (A * first[i] + B * first[i + 1]) * exp(-t/n);
	}
	//mtx.unlock();
}

double BinomialPrice(std::vector<double> data, std::string opType, int threads)
{
	double S = data[0];
	double K = data[1];
	double r = data[2];
	double q = data[3];
	double t = data[4];
	double v = data[5];
	double n = data[6];

	double P = Jump(v, t, n, false);
	double D = Jump(v, t, n, true);

	double A = Prob(r, t, n, P, D);
	double B = 1 - A;
	
	int m = (int) n + 1;
	
	double dx = floor(m / threads);
	
	int a = 0, b = dx;
	
	std::vector<std::thread> init_comp;
	
	double * temp = new double[m];
	
	for(int i = 0; i < threads; ++i)
	{
		if(i == threads - 1 && b < m){
			b = m;
		}
		
		init_comp.emplace_back(Spread, std::ref(temp), S, K, P, D, m, a, b, opType);
		
		a += dx;
		b += dx;
	}
	
	
	for(auto& u : init_comp)
	{
		u.join();
	}
	
	init_comp.clear();
	
	
	for(int j = n; j > 0; --j)
	{
		dx = floor(j / threads); 
		a = 0; b = dx;
		
		for(int i = 0; i < threads; ++i)
		{
			if(i == threads - 1 && b < j){
				b = j;
			}
						
			init_comp.emplace_back(Discounter, std::ref(temp), A, B, t, n, a, b);  
			
			a += dx;
			b += dx;
		}
		
		for(auto & u : init_comp)
		{
			u.join();
		}
		
		init_comp.clear();
	}
	
	double price = temp[0];
	
	return price;
}

int main()
{
	double S = 115;
	double K = 120;
	double r = 0.05;
	double q = 0.02;
	double t = 0.35;
	double v = 0.45;
	double n = 8500;	
	
	for(int i = 1; i < 10; ++i)
	{
		int t0 = nonce();
		double premium = BinomialPrice({S,K,r,q,t,v,n},"Call",i);
		if(i % 2 != 0)
            std::cout << "Nodes: " << n << "\tThreads: " << i << "\tTime: " << nonce() - t0 << "\tOption Premium: " << premium <<  std::endl;	
	}
	return 0;
}