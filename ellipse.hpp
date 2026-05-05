#ifndef ELLIPSE_HPP
#define ELLIPSE_HPP

#include <cmath>
#include <utility>
#include <vector>
#include <algorithm>

class myEllipse {
private:
    std::vector<std::pair<int, int>> ellipse;
    std::pair<int, int> center, a_, b_;
    char m_;
    std::vector<double> factorials;
    std::vector<double> powers;

    // Generates powers of theta up to theta^10
    void compute_powers(double theta) {
        powers.clear();
        powers.push_back(1.0); // theta^0
        for (int i = 1; i <= 10; i++) {
            powers.push_back(theta * powers.back());
        }
    }

    // Generates factorials up to number!
    void compute_factorials(int number) {
        factorials.clear();
        factorials.push_back(1.0); // 0! is 1
        for (int i = 1; i <= number; i++) {
            factorials.push_back(i * factorials.back());
        }
    }

    std::vector<std::pair<int, int>> direct(std::pair<int, int> c,
                                            std::pair<int, int> a,
                                            std::pair<int, int> b) {
        double a2 = (double)a.first * a.first;
        int x = 0;

        while (x <= a.first) {
            int y = std::round(b.second * std::sqrt(1.0 - ((double)x * x) / a2));
            
            // Symmetry points
            ellipse.push_back({ c.first + x, c.second + y });
            ellipse.push_back({ c.first - x, c.second + y });
            ellipse.push_back({ c.first + x, c.second - y });
            ellipse.push_back({ c.first - x, c.second - y });

            x++;
        }
        return ellipse;
    }

    std::vector<std::pair<int, int>> polar(std::pair<int, int> c,
                                           std::pair<int, int> a,
                                           std::pair<int, int> b) {
        double rx = a.first;
        double ry = b.second;
        
        // Pre-calculate factorials once to save CPU
        compute_factorials(10);

        // Determine step size based on circumference approximation
        double num_steps = std::max(rx, ry) * 2; 
        double d_theta = (M_PI / 2.0) / num_steps; 
        double theta = 0;

        for (int i = 0; i <= num_steps; i++) {
            compute_powers(theta);

            // Taylor Series for Sin and Cos
            double s_val = powers[1] - (powers[3]/factorials[3]) + (powers[5]/factorials[5]) - (powers[7]/factorials[7]) + (powers[9]/factorials[9]);
            double c_val = powers[0] - (powers[2]/factorials[2]) + (powers[4]/factorials[4]) - (powers[6]/factorials[6]) + (powers[8]/factorials[8]);

            int x_p = std::round(rx * c_val); 
            int y_p = std::round(ry * s_val); 

            // Symmetry points for all four quadrants
            ellipse.push_back({ c.first + x_p, c.second + y_p });
            ellipse.push_back({ c.first - x_p, c.second + y_p });
            ellipse.push_back({ c.first + x_p, c.second - y_p });
            ellipse.push_back({ c.first - x_p, c.second - y_p });

            theta += d_theta;
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

        // Ensure a and b are relative distances from center
        // if they were passed as absolute coordinates
        a.first = std::abs(a.first - c.first);
        b.second = std::abs(b.second - c.second);

        switch (m) {
            case 'd':
                return direct(c, a, b);
            case 'p':
                return polar(c, a, b);
            default:
                break;
        }

        return ellipse;
    }
};

#endif
