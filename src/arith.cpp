// Copyright (c) 2018-2019  Robert J. Hijmans
//
// This file is part of the "spat" library.
//
// spat is free software: you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 2 of the License, or
// (at your option) any later version.
//
// spat is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with spat. If not, see <http://www.gnu.org/licenses/>.

#include <functional>
#include "spatRaster.h"
#include "recycle.h"


// need to take care of NAs here. OK for NAN, but not for int types
template <typename T>
void operator+(std::vector<T>& a, const std::vector<T>& b) {
    std::transform(a.begin(), a.end(), b.begin(), a.begin(), std::plus<T>());
}


template <typename T>
void operator-(std::vector<T>& a, const std::vector<T>& b) {
    std::transform(a.begin(), a.end(), b.begin(), a.begin(), std::minus<T>());
}


template <typename T>
void operator/(std::vector<T>& a, const std::vector<T>& b) {
    std::transform(a.begin(), a.end(), b.begin(), a.begin(), std::divides<T>());
}

template <typename T>
void operator*(std::vector<T>& a, const std::vector<T>& b) {
    std::transform(a.begin(), a.end(), b.begin(), a.begin(), std::multiplies<T>());
}



template <typename T>
void operator%(std::vector<T>& a, const std::vector<T>& b) {
//    std::transform(a.begin(), a.end(), b.begin(), a.begin(), std::modulus<T>());
	for (size_t i=0; i<a.size(); i++) {
		if (std::isnan(a[i]) || std::isnan(b[i])) {
			a[i] = NAN;
		} else {
			a[i] = std::fmod(a[i], b[i]);
		}
	}
}


template <typename T>
void operator==(std::vector<T>& a, const std::vector<T>& b) {
//    std::transform(a.begin(), a.end(), b.begin(), a.begin(), std::equal_to<T>());
	for (size_t i=0; i<a.size(); i++) {
		if (std::isnan(a[i]) || std::isnan(b[i])) {
			a[i] = NAN;
		} else {
			a[i] = a[i] == b[i];
		}
	}
}

template <typename T>
void operator!=(std::vector<T>& a, const std::vector<T>& b) {
//    std::transform(a.begin(), a.end(), b.begin(), a.begin(), std::not_equal_to<T>());
	for (size_t i=0; i<a.size(); i++) {
		if (std::isnan(a[i]) || std::isnan(b[i])) {
			a[i] = NAN;
		} else {
			a[i] = a[i] != b[i];
		}
	}
}

template <typename T>
void operator>=(std::vector<T>& a, const std::vector<T>& b) {
//    std::transform(a.begin(), a.end(), b.begin(), a.begin(), std::greater_equal<T>());
	for (size_t i=0; i<a.size(); i++) {
		if (std::isnan(a[i]) || std::isnan(b[i])) {
			a[i] = NAN;
		} else {
			a[i] = a[i] >= b[i];
		}
	}
}

template <typename T>
void operator<=(std::vector<T>& a, const std::vector<T>& b) {
//    std::transform(a.begin(), a.end(), b.begin(), a.begin(), std::less_equal<T>());
	for (size_t i=0; i<a.size(); i++) {
		if (std::isnan(a[i]) || std::isnan(b[i])) {
			a[i] = NAN;
		} else {
			a[i] = a[i] <= b[i];
		}
	}
 }


template <typename T>
void operator>(std::vector<T>& a, const std::vector<T>& b) {
//    std::transform(a.begin(), a.end(), b.begin(), a.begin(), std::greater<T>());
	for (size_t i=0; i<a.size(); i++) {
		if (std::isnan(a[i]) || std::isnan(b[i])) {
			a[i] = NAN;
		} else {
			a[i] = a[i] > b[i];
		}
	}
}

template <typename T>
void operator<(std::vector<T>& a, const std::vector<T>& b) {
//    std::transform(a.begin(), a.end(), b.begin(), a.begin(), std::less<T>());
	for (size_t i=0; i<a.size(); i++) {
		if (std::isnan(a[i]) || std::isnan(b[i])) {
			a[i] = NAN;
		} else {
			a[i] = a[i] < b[i];
		}
	}
}


template <typename T>
void power(std::vector<T>& a, const std::vector<T>& b) {
	for (size_t i=0; i<a.size(); i++) {
		if (std::isnan(a[i]) || std::isnan(b[i])) {
			a[i] = NAN;
		} else {
			a[i] = std::pow(a[i], b[i]);
		}
	}
}


bool smooth_operator(std::string oper) {
	std::vector<std::string> f {"+", "-", "*", "^", "/", "%", "==", "!=", ">", "<", ">=", "<="};
	return (std::find(f.begin(), f.end(), oper) != f.end());
}



SpatRaster SpatRaster::arith(SpatRaster x, std::string oper, SpatOptions &opt) {

	size_t nl = std::max(nlyr(), x.nlyr());
	SpatRaster out = geometry(nl);


	if (!smooth_operator(oper)) {
		out.setError("unknown arith function");
		return out;
	}

	if (!compare_geom(x, false, true)) {
		out.setError("dimensions and/or extent do not match");
		return(out);
	}
	
	if (!(hasValues() & x.hasValues())) {
		out.setError("raster has no values"); // or warn and treat as NA?
		return out;
	}
 	if (!out.writeStart(opt)) { return out; }
	readStart();
	x.readStart();
	for (size_t i = 0; i < out.bs.n; i++) {
		std::vector<double> a = readBlock(out.bs, i);
		std::vector<double> b = x.readBlock(out.bs, i);
		recycle(a,b);
		if (oper == "+") {
			a + b;
		} else if (oper == "-") {
			a - b;
		} else if (oper == "*") {
			a * b;
		} else if (oper == "/") {
			a / b;
		} else if (oper == "^") {
			power(a, b);
		} else if (oper == "%") {
			 a % b;
		} else if (oper == "==") {
			a == b;
		} else if (oper == "!=") {
			a == b;
		} else if (oper == ">=") {
			a >= b;
		} else if (oper == "<=") {
			a <= b;
		} else if (oper == ">") {
			a > b;
		} else if (oper == "<") {
			a < b;
		}
		if (!out.writeValues(a, out.bs.row[i], out.bs.nrows[i], 0, ncol())) return out;

	}
	out.writeStop();
	readStop();
	x.readStop();
	return(out);
}



SpatRaster SpatRaster::arith(double x, std::string oper, bool reverse, SpatOptions &opt) {

	SpatRaster out = geometry();
	if (!smooth_operator(oper)) {
		out.setError("unknown arith function");
		return out;
	}	
	if (!hasValues()) {
		out.setError("raster has no values"); // or warn and treat as NA?
		return out;
	}

  	if (!out.writeStart(opt)) { return out; }
	readStart();
	for (size_t i = 0; i < out.bs.n; i++) {
		std::vector<double> a = readBlock(out.bs, i);
		if (std::isnan(x)) {
			for(double& d : a)  d = NAN;
		} else if (oper == "+") {
			for(double& d : a)  d += x;
		} else if (oper == "-") {
			if (reverse) {
				for(double& d : a)  d = x - d;
			} else {
				for(double& d : a)  d -= x;
			}
		} 
		else if (oper == "*") {
			for(double& d : a)  d *= x;
		} else if (oper == "/") {
			if (reverse) {
				for(double& d : a)  d = x / d;
			} else {
				for(double& d : a)  d /= x;
			}
		} else if (oper == "^") {
			if (reverse) {
				for(double& d : a)  d = std::pow(x, d);				
			} else {
				for(double& d : a)  d = std::pow(d, x);
			}
		} else if (oper == "%") {
			if (reverse) {
				for(double& d : a) std::fmod(x, d);
			} else {
				for(double& d : a) std::fmod(d, x);
			}
		} else if (oper == "==") {
			for(double& d : a) if (!std::isnan(d)) d = d == x;
		} else if (oper == "!=") {
			for(double& d : a) if (!std::isnan(d)) d = d != x;
		} else if (oper == ">=") {
			if (reverse) {
				for(double& d : a) if (!std::isnan(d)) d = x >= d;				
			} else {
				for(double& d : a) if (!std::isnan(d)) d = d >= x;
			}
		} else if (oper == "<=") {
			if (reverse) {
				for(double& d : a) if (!std::isnan(d)) d = x <= d;				
			} else {
				for(double& d : a) if (!std::isnan(d)) d = d <= x;
			}
		} else if (oper == ">") {
			if (reverse) {
				for(double& d : a) if (!std::isnan(d)) d = x > d;				
			} else {
				for(double& d : a) if (!std::isnan(d)) d = d > x;
			}
		} else if (oper == "<") {
			if (reverse) {
				for(double& d : a) if (!std::isnan(d)) d = x < d;				
			} else {
				for(double& d : a) if (!std::isnan(d)) d = d < x;
			}
		} else {
			// stop
		}
		if (!out.writeValues(a, out.bs.row[i], out.bs.nrows[i], 0, ncol())) return out;
	}
	out.writeStop();
	readStop();
	return(out);
}



SpatRaster SpatRaster::arith(std::vector<double> x, std::string oper, bool reverse, SpatOptions &opt) {

	if (x.size() == 1) {
		return(arith(x[0], oper, reverse, opt));	
	}
	
	SpatRaster out = geometry();
	if (!smooth_operator(oper)) {
		out.setError("unknown arith function");
		return out;
	}
	if (!hasValues()) {
		out.setError("raster has no values"); // or warn and treat as NA?
		return out;
	}

  	if (!out.writeStart(opt)) { return out; }
	readStart();

	unsigned nl = nlyr();
	unsigned nc = ncol();
	recycle(x, nlyr());
	
	for (size_t i = 0; i < out.bs.n; i++) {
		std::vector<double> v = readBlock(out.bs, i);
		std::vector<double> vv;
		unsigned off = out.bs.nrows[i] * nc;
		for (size_t j=0; j<nl; j++) {
			unsigned s = j * off;
			std::vector<double> a(v.begin()+s, v.begin()+s+off);
			if (std::isnan(x[j])) {
				for(double& d : a) d = NAN;
			} else if (oper == "+") {
				for(double& d : a) d += x[j];
			} else if (oper == "-") {
				if (reverse) {
					for(double& d : a) d = x[j] - d;
				} else {
					for(double& d : a) d -= x[j];
				}
			} else if (oper == "*") {
				for(double& d : a)  d *= x[j];
			} else if (oper == "/") {
				if (reverse) {
					for(double& d : a) d = x[j] / d;
				} else {
					for(double& d : a) d /= x[j];
				}
			} else if (oper == "^") {
				if (reverse) {
					for(double& d : a)  d = std::pow(x[j], d);				
				} else {
					for(double& d : a)  d = std::pow(d, x[j]);
				}
			} else if (oper == "%") {
				if (reverse) {
					for(double& d : a) std::fmod(x[j], d);
				} else {
					for(double& d : a) std::fmod(d, x[j]);
				}
			} else if (oper == "==") {
				for(double& d : a) if (!std::isnan(d)) d = d == x[j];
			} else if (oper == "!=") {
				for(double& d : a) if (!std::isnan(d)) d = d != x[j];
			} else if (oper == ">=") {
				if (reverse) {
					for(double& d : a) if (!std::isnan(d)) d = x[j] >= d;				
				} else {
					for(double& d : a) if (!std::isnan(d)) d = d >= x[j];
				}
			} else if (oper == "<=") {
				if (reverse) {
					for(double& d : a) if (!std::isnan(d)) d = x[j] <= d;				
				} else {
					for(double& d : a) if (!std::isnan(d)) d = d <= x[j];
				}
			} else if (oper == ">") {
				if (reverse) {
					for(double& d : a) if (!std::isnan(d)) d = x[j] > d;				
				} else {
					for(double& d : a) if (!std::isnan(d)) d = d > x[j];
				}
			} else if (oper == "<") {
				if (reverse) {
					for(double& d : a) if (!std::isnan(d)) d = x[j] < d;				
				} else {
					for(double& d : a) if (!std::isnan(d)) d = d < x[j];
				}
			} else {
				// stop
			}
			vv.insert(vv.end(), a.begin(), a.end());
		}
		if (!out.writeValues(vv, out.bs.row[i], out.bs.nrows[i], 0, ncol())) return out;
	}
	out.writeStop();
	readStop();
	return(out);
}



