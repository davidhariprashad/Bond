#ifndef BOND_HPP
#define BOND_HPP

#include <iostream>
#include <iomanip>
#include <vector>
#include <cmath>

class Bond
{
	friend std::ostream& operator<<(std::ostream& os, const Bond& bond);
public:
	Bond(double F, double issue_date, int num_periods, int freq, const std::vector<double>& c);
	~Bond();
	void setFlatCoupons(double c);
	void setCoupons(const std::vector<double>& c);
	double FairValue(double t0, double y) const;
	double maturity() const {return _maturity;}
	double issue() const {return _issue;}
	int FV_duration(double t0, double y, double& B, double& Mac_dur, double& mod_dur) const;
private:
	double _Face;
	double _issue;
	double _maturity;
	int    _cpnFreq;
	int    _numCpnPeriods;
	std::vector<double> _cpnAmt;
	std::vector<double> _cpnDate;
};

int yield(double& y, int& num_iter, const Bond& bond, double B_target, double t0, double tol = 1.0e-4, int max_iter = 100);

#endif
