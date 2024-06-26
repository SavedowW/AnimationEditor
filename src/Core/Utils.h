#ifndef UTILS_H_
#define UTILS_H_
#include <filesystem>
#include <string>
#include <algorithm>

namespace utils
{
    template<typename T>
    class Average
    {
    public:
        Average(const T &def_ = 0) :
            sum(def_)
        {
        }

        template<typename T2>
        Average &operator+=(const T2 &rhs_)
        {
            sum += rhs_;
            cnt++;
            return *this;
        }

        template<typename T2>
        operator T2()
        {
            return sum / cnt;
        }

    private:
        T sum;
        int cnt = 0;

    };

    template <typename T>
    inline T clamp(const T& val, const T &min, const T &max)
    {
    	if (val < min)
    		return min;
    
    	if (val > max)
    		return max;
    
    	return val;
    }

    template <typename T>
    inline bool sameSign(const T &v1, const T &v2)
    {
    	return (v1 > 0 && v2 > 0 || v1 < 0 && v2 < 0);
    }

    template <typename T>
    inline T reverseLerp(const T& val, const T &min, const T &max)
    {
    	T alpha = (val - min) / (max - min);
        return clamp<T>(alpha, 0, 1);
    }

    inline std::string getRelativePath(const std::string &basePath_, const std::string &fullPath_)
	{
		std::filesystem::path path1(basePath_);
    	std::filesystem::path path2(fullPath_);
    	auto p = std::filesystem::relative(path2, path1).string();
    	std::replace(p.begin(), p.end(), '\\', '/');
    	return p;
	}
}

template <typename T, size_t len>
std::ostream& operator<< (std::ostream& out, const std::array<T, len>& arr)
{
    std::cout << "[";
    for (int i = 0; i < len; ++i)
    {
        std::cout << arr[i];
        if (i != len - 1)
            std::cout << ", ";
    }
    std::cout << "]";
    return out;
}

#endif