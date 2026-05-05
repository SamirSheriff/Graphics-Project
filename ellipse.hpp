#ifndef ELLIPSE_HPP
#define ELLIPSE_HPP

#include <cmath>
#include <utility>
#include <vector>

class myEllipse {
private:
    std::vector<std::pair<int, int>> ellipse;
    std::pair<int, int> center, a_, b_;
    char m_;

    std::vector<std::pair<int, int>> direct(std::pair<int, int> c,
                                            std::pair<int, int> a,
                                            std::pair<int, int> b) {
        double a2 = (double)a.first * a.first;
        int x = 0;

        while (x <= a.first) {
            int y = std::round(b.second * std::sqrt(1.0 - ((double)x * x) / a2));
	    ellipse.push_back({c.first, c.second});
            ellipse.push_back({ c.first + x, c.second + y });
            ellipse.push_back({ c.first - x, c.second + y });
            ellipse.push_back({ c.first + x, c.second - y });
            ellipse.push_back({ c.first - x, c.second - y });

            x++;
        }
        return ellipse;
    }

public:
    std::vector<std::pair<int, int>> draw_ellipse(std::pair<int, int> c,
                                                  std::pair<int, int> a,
                                                  std::pair<int, int> b,
                                                  char m) {
        center = c;
        a_ = a;
        b_ = b;
        m_ = m;
        ellipse.clear();

        a.second = c.second;
        b.first = c.first;

        switch (m) {
            case 'd':
                return direct(c, a, b);
        }

        return ellipse;
    }
};

#endif
