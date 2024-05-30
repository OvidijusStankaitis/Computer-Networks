# Constants provided in the problem
n = 220
a = 2.9
x = 2.1
epsilon = 0.05

# Calculating the CDF of X at x
F_X_x = x / a

# Calculating the variance of Fn(x)
var_Fn_x = (F_X_x * (1 - F_X_x)) / n

# Using Chebyshev's Inequality
chebyshev_bound = var_Fn_x / (epsilon ** 2)

# Using the Central Limit Theorem
# Standardizing the deviation
z_value = epsilon / (var_Fn_x ** 0.5)

# Using a Z-table or normal distribution to find the probability that |Z| >= z_value
# This uses the symmetry of the normal distribution and 1 - P(|Z| < z_value)
from scipy.stats import norm

clt_probability = 2 * (1 - norm.cdf(z_value))

print(chebyshev_bound, clt_probability)