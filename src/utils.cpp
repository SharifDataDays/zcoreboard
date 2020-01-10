#include <string>
#include <sstream>
#include <iomanip>

using namespace std;

std::string score_to_string(float score) {
	stringstream stream;
	stream << fixed << setprecision(2) << score;
	return stream.str();
}
