# <font color='red'>Math library</font> #
## <font color='midnightblue'>basic template's</font> ##
<font color='midnightblue'>
<b>Main changes:</b>
<ul><li>All classes and functions turned to templates<br>
</li><li>Intermediate computations removed, in all possible cases.<br> For instance:<br> Previously the <b>line</b> was defined as vector <b>a</b> and vector <b>e</b>. Vector <b>e</b> was computed as vector's <b>b</b> - <b>a</b> difference (<b>a</b> and <b>b</b> are original ends of the segment).<br> Hence, in case it was used as a segment, <b>a + e</b> was computed to get second end of the segment. Now <b>line</b> keeps both <b>a</b> and <b>b</b> to avoid this computation and resulted floating point fluctuations. Those fluctuations are result of the conversion of float/double 32/64-bit values to Intel CPU FPU's 80 bit registers.<br>
</li><li>Comparisons of vector's are firm, <b>memcmp</b> is used.<br>
</li><li>The triangle's edges always used in</li></ul>

<h3>I. mathlib/nmath.h - base constants and functions. Many of templates has an optimized type specific implementations.</h3>
<pre><code>typedef float nReal<br>
</code></pre>
<b>constants:</b>
<pre><code>N_PI, N_HALF_PI, N_DOUBLE_PI, N_LN_2 ...<br>
</code></pre>
<b>comparisons, relations, fuzzy:</b>
<pre><code>n_max(a, b)/n_min(a, b), n_max3(a, b, c)/n_min3(a, b, c)<br>
<br>
n_abs(f), ///&lt; float and double versions has optimized implementation<br>
n_swap(a, b)<br>
n_half(a) ///&lt; integer types has optimized implementation<br>
n_eqz(x, tol = def)/n_neqz(x, tol = def), n_eq(a, b, tol = def)/n_neq(a, b, tol = def)<br>
n_sgn(f , tol = def) <br>
n_inv( f)<br>
</code></pre>
<b>ranges:</b>
<pre><code>n_inrange(a, lower, upper, tol = def)<br>
n_in1(a, tol = def)<br>
n_clamp(a, lower, upper)<br>
n_saturate(a) <br>
n_lerp(a, b, l) ///&lt; interpolation<br>
</code></pre>
<b>trigonometric and basic math. :</b>
<pre><code>n_deg2rad(d), n_rad2deg(r)<br>
n_sin(x), n_cos(x), n_tan(x), n_atan(x), n_atan2(x)<br>
n_acos(x), n_asin(x) ///&lt; clamping x<br>
<br>
n_exp(x), n_floor(x),n_ceil(x), n_pow(x)<br>
n_log2(x), n_sqrt(x), n_sqr(x)<br>
n_smooth(new, last, max_d)<br>
n_rand() ///&lt; random in [0,1]<br>
n_rand(min, max) ///&lt; random in [min, max]<br>
long n_ftol(float) ///&lt; fast<br>
long n_ftol(double)///&lt; fast<br>
</code></pre>
<h3>II. mathlib/vector.h -  2,3,4 component vector templates, used mostly for the vertex coordinates</h3>

<h3>III. mathlib/line.h -  vector based line and derivatives like ray, segment etc.</h3>
<h3>IV. mathlib/plane.h -  2,3,4 component vector, used mostly for the vertex coordinates</h3>
<h3>V. mathlib/triangle.h -  2,3,4 component vector, used mostly for the vertex coordinates</h3>
<h3>VI. mathlib/quaternion.h -  2,3,4 component vector, used mostly for the vertex coordinates</h3>
<h3>VII. mathlib/matrix.h -  2,3,4 component vector, used mostly for the vertex coordinates</h3>