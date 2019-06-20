#include "Bond.hpp"

std::ostream& operator<<(std::ostream& os, const Bond& bond)
{
	os << "_Face = $" << bond._Face << std::endl;
	os << "_issue = " << bond._issue << std::endl;
	os << "_maturity = " << bond._maturity << std::endl;
	os << "_cpnFreq = " << bond._cpnFreq << std::endl;
	os << "_numCpnPeriods = " << bond._numCpnPeriods << std::endl;

	const int w = 10;
	os << std::setw(w) << "index";
	os << std::setw(w) << "_cpnDate";
	os << std::setw(w) << "_cpnAmt" << std::endl;
	for (int i = 0u; i < bond._numCpnPeriods; ++i)
	{
		os << std::setw(w) << i;
		os << std::setw(w) << bond._cpnDate[i];
		os << std::setw(w) << bond._cpnAmt[i] << std::endl;
	}

	return os;
}

Bond::Bond(double F, double issue_date, int num_periods, int freq, const std::vector<double>& c)
{
	_Face = (F < 0.0 ? 0.0 : F);
	_cpnFreq = (freq < 1 ? 1 : freq);
	_numCpnPeriods = (num_periods < 1 ? 1 : num_periods);
	_issue = issue_date;
	_maturity = _issue + static_cast<double>(_numCpnPeriods) / _cpnFreq;
	_cpnAmt.resize(_numCpnPeriods);
	_cpnDate.resize(_numCpnPeriods);
	for (unsigned int i = 0u; i < _cpnDate.size(); ++i)
		{_cpnDate[i] = _issue + static_cast<double>(i + 1) / _cpnFreq;}
	setCoupons(c);
}

Bond::~Bond() {}

void Bond::setFlatCoupons(double c)
{
	if (c < 0.0) c = 0.0;
	std::fill(_cpnAmt.begin(), _cpnAmt.end(), c);
}

void Bond::setCoupons(const std::vector<double>& c)
{
	if (c.size() == 0u)
	{
		std::fill(_cpnAmt.begin(), _cpnAmt.end(), 0.0);
		return;
	}

	unsigned int i;
	for (i = 0u; (i < _cpnAmt.size() && (i < c.size())); ++i)
		_cpnAmt[i] = (c[i] < 0.0 ? 0.0 : c[i]);
	double last = (c.back() < 0.0 ? 0.0 : c.back());
	for(; i < _cpnAmt.size(); ++i)
		_cpnAmt[i] = last;
}

double Bond::FairValue(double t0, double y) const
{
	double B = 0.0;
	double dummy1 = 0.0;
	double dummy2 = 0.0;
	FV_duration(t0, y, B, dummy1, dummy2);
	return B;
}

int Bond::FV_duration(double t0, double y, double& B, double& Mac_dur, double& mod_dur) const
{
	B = 0.0;
	Mac_dur = 0.0;
	mod_dur = 0.0;
	if ((t0 < _issue) || (t0 >= _maturity)) return 1;

	const double y_decimal = 0.01 * y;
	const double u = 1.0 + y_decimal / _cpnFreq;
	const double tol = 1.0e-6;
	double denominator;
	for (int i = 0; i < _numCpnPeriods; ++i)
	{
		if (_cpnDate[i] >= t0 + tol)
		{
			denominator = std::pow(u, _cpnFreq * (_cpnDate[i] - t0));
			B += _cpnAmt[i] / _cpnFreq / denominator;
			Mac_dur += (_cpnDate[i] - t0) * _cpnAmt[i] / _cpnFreq / denominator;
		}
	}
	B += _Face / denominator;
	Mac_dur += (_cpnDate.back() - t0) * _Face / denominator;
	Mac_dur /= B;
	mod_dur = Mac_dur / u;

	return 0;
}

int yield(double& y, int& num_iter, const Bond& bond, double B_target, double t0, double tol, int max_iter)
{
	y = 0.0;

	if ((B_target < 0.0) || (t0 < bond.issue()) || (t0 >= bond.maturity())) return 1;

	double y_low = 0.0;
	double y_high = 100.0;

	double B_y_low = bond.FairValue(t0, y_low);
	double diff_B_y_low = B_y_low - B_target;
	if (std::abs(diff_B_y_low) <= tol)
	{
		y = y_low;
		return 0;
	}

	double B_y_high = bond.FairValue(t0, y_high);
	double diff_B_y_high = B_y_high - B_target;
	if (std::abs(diff_B_y_high) <= tol)
	{
		y = y_high;
		return 0;
	}

	if (diff_B_y_low * diff_B_y_high > 0.0)
	{
		y = 0;
		return 1;
	}

	double B, diff_B;
	for (num_iter = 1; num_iter < max_iter; ++num_iter)
	{
		y = (y_low + y_high) / 2.0;
		B = bond.FairValue(t0, y);
		diff_B = B - B_target;
		if (std::abs(diff_B) <= tol)
			return 0;

		if (diff_B * diff_B_y_low > 0.0)
			y_low = y;
		else
			y_high = y;

		if (std::abs(y_high - y_low) < tol)
			return 0;
	}

	y = 0;
	return 1;
}
