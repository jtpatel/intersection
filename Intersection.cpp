#include <iostream>
#include <cmath>
#include <utility>
#include <vector>
#include <memory>

const auto sqr = [](auto x) { return x * x; };

class Shape
{
public:
	virtual bool intersect(Shape* target) = 0;
	virtual ~Shape() {}
};

class Circle : public Shape
{
	double m_X = 0.0;
	double m_Y = 0.0;
	double m_Radius = 0.0;
public:
	Circle(double X, double Y, double Radius) :m_X{ X }, m_Y{ Y }, m_Radius{ Radius }{}
	~Circle() {}
	bool intersect(Shape* target);
	double GetX() { return m_X; }
	double GetY() { return m_Y; }
	double GetRadius() { return m_Radius; }
};

class Rectangle : public Shape
{
	double m_X = 0.0;
	double m_Y = 0.0;
	double m_Width = 0.0;
	double m_Height = 0.0;
public:
	Rectangle(double X, double Y, double Width, double Height) :m_X{ X }, m_Y{ Y }, m_Width{ Width }, m_Height{ Height }{}
	~Rectangle() {}
	bool intersect(Shape* target);
	double GetX() { return m_X; }
	double GetY() { return m_Y; }
	double GetWidth() { return m_Width; }
	double GetHeight() { return m_Height; }
};

class Vehicle : public Shape
{
	std::string m_Name = "";
public:
	std::vector<std::unique_ptr<Shape>> objects;// use smart pts
	Vehicle(const std::string& name) : m_Name{ name } {}
	~Vehicle() {}
	bool intersect(Shape* target);
};

bool CheckIntersection(Circle* icircle1, Circle* icircle2)
{
	if (sqr(icircle1->GetX() - icircle2->GetX()) + sqr(icircle1->GetY() - icircle2->GetY())
		< sqr(icircle1->GetRadius() + icircle2->GetRadius()))
		return true;
	
	return false;
}

bool CheckIntersection(Rectangle * irect1, Rectangle * irect2)
{
	const auto max_distance_width = (irect1->GetWidth() / 2.0) + (irect2->GetWidth() / 2.0);
	const auto max_distance_height = (irect1->GetHeight() / 2.0) + (irect2->GetHeight() / 2.0);
	if (std::abs(irect1->GetX() - irect2->GetX()) < max_distance_width
		&& std::abs(irect1->GetY() - irect2->GetY()) < max_distance_height)
		return true;
	
	return false;
}

bool CheckIntersection(Circle * icircle, Rectangle * irect)
{
	double car_x = irect->GetX();
	double car_y = irect->GetY();
	double halfheight = irect->GetHeight() / 2.0;
	double halfwidth = irect->GetWidth() / 2.0;

	double other_car_x = icircle->GetX();
	double other_car_y = icircle->GetY();
	double other_halfheight = icircle->GetRadius();
	double other_halfwidth = other_halfheight;

	// Step 1: point-in-circle test of corners
	const auto corners = std::vector<std::pair<double, double>>{
	{ car_x - halfwidth, car_y - halfheight },
	{ car_x - halfwidth, car_y + halfheight },
	{ car_x + halfwidth, car_y - halfheight },
	{ car_x + halfwidth, car_y + halfheight },
	};

	for (const auto [corner_x, corner_y] : corners)
	{
		if (sqr(corner_x - other_car_x) + sqr(corner_y - other_car_y)
			< sqr(other_halfwidth))
		{
			return true;
		}
	}

	// Step 2: point-in-rectangle test of extremes of circle
	const auto extremes = std::vector<std::pair<double, double>>{
	{ other_car_x - other_halfwidth, other_car_y },
	{ other_car_x + other_halfwidth, other_car_y },
	{ other_car_x, other_car_y - other_halfheight },
	{ other_car_x, other_car_y + other_halfheight },

	// Add center-point, which detects fully overlapping shapes.
	// In case the sizes are equivalent, the extremes do not fall
	// inside the rectangle.
	{ other_car_x, other_car_y },
	};

	for (const auto [extrema_x, extrema_y] : extremes)
	{
		if ((car_x - halfwidth < extrema_x && extrema_x < car_x + halfwidth)
			&& (car_y - halfheight < extrema_y && extrema_y < car_y + halfheight))
		{
			return true;
		}
	}
	return false;
}


bool Circle::intersect(Shape * target)
{
	//between circle - circle
	Circle* tarAsCircle = dynamic_cast<Circle*>(target);
	if (tarAsCircle)
	{
		return CheckIntersection(this, tarAsCircle);
	}

	//between circle - rectangle
	Rectangle* tarAsRect = dynamic_cast<Rectangle*>(target);
	if (tarAsRect)
	{
		return CheckIntersection(this, tarAsRect);
	}

	//between circle - Vehicle
	Vehicle* tarAsVehicle = dynamic_cast<Vehicle*>(target);
	if (tarAsVehicle)
	{
		//check circle Vehicle intersects
		for (int i = 0; i < tarAsVehicle->objects.size(); i++)
			if (this->intersect(tarAsVehicle->objects.at(i).get()))
				return true;
	}
	return false;
}

bool Rectangle::intersect(Shape * target)
{
	//between rectangle - circle
	Circle* tarAsCircle = dynamic_cast<Circle*>(target);
	if (tarAsCircle)
	{
		return CheckIntersection(tarAsCircle, this);
	}

	//between rectangle - rectangle
	Rectangle* tarAsRect = dynamic_cast<Rectangle*>(target);
	if (tarAsRect)
	{
		return CheckIntersection(this, tarAsRect);
	}

	//between rectangle - Vehicle
	Vehicle* tarAsVehicle = dynamic_cast<Vehicle*>(target);
	if (tarAsVehicle)
	{
		//check rectangle Vehicle intersects
		for (int i = 0; i < tarAsVehicle->objects.size(); i++)
			if (this->intersect(tarAsVehicle->objects.at(i).get()))
				return true;
	}
	return false;
}

bool Vehicle::intersect(Shape * target)
{
	for (int i = 0; i < objects.size(); i++)
	{
		if (objects.at(i)->intersect(target))
			return true;
	}
}

int main()
{
	Vehicle v1{ "A" };
	v1.objects.emplace_back(new Circle(0.0, 0.0, 3.0));
	v1.objects.emplace_back(new Circle(10.0, 0.0, 3.0));
	v1.objects.emplace_back(new Rectangle(5.0, 3.0, 10.0, 6.0));

	Vehicle v2{ "B" };
	v2.objects.emplace_back(new Circle(0.0, 25.0, 3.0));
	v2.objects.emplace_back(new Circle(10.0, 25.0, 3.0));
	v2.objects.emplace_back(new Rectangle(5.0, 28.0, 10.0, 6.0));

	bool result = v1.intersect(&v2);

	std::cout << ((result == true) ? "true" : "false") << std::endl;

	getchar();
}

#ifdef Shared_Code
#pragma region Shared_Code
// Computes for a list of cars, which of them intersect each other.
//
// Note: the cars are currently approximated by rectangles or circles.
//
// The arguments 'cars_x' and 'cars_y' are two vectors of center locations
// of the rectangles/circles. The arguments 'cars_width' and
// 'cars_height' are two list of the widths and heights of the cars.
// The argument 'cars_is_circular' is a vector containing a boolean
// value for each car, indicating whether it's a circle (true)
// or a square (false).
auto get_intersections(
	const std::vector<double> & cars_x,
	const std::vector<double> & cars_y,
	const std::vector<double> & cars_width,
	const std::vector<double> & cars_height,
	const std::vector<bool> & cars_is_circular)
{
	auto result = std::vector<std::pair<std::size_t, std::size_t>>{};

	for (auto i = std::size_t{ 0 }; i != cars_x.size(); ++i)
	{
		for (auto j = std::size_t{ 0 }; j != cars_x.size(); ++j)
		{
			if (j <= i) continue;

			auto car_x = cars_x[i];
			auto other_car_x = cars_x[j];

			auto car_y = cars_y[i];
			auto other_car_y = cars_y[j];

			auto halfwidth = cars_width[i] / 2;
			auto other_halfwidth = cars_width[j] / 2;


			auto halfheight = cars_height[i] / 2;
			auto other_halfheight = cars_height[j] / 2;

			auto car_is_circular = cars_is_circular[i];
			auto other_car_is_circular = cars_is_circular[j];

			const auto sqr = [](auto x) { return x * x; };

			if (car_is_circular && other_car_is_circular)
			{
				if (sqr(car_x - other_car_x) + sqr(car_y - other_car_y)
					< sqr(halfwidth + other_halfwidth))
					result.emplace_back(i, j);
			}
			else if (!car_is_circular && !other_car_is_circular)
			{
				const auto max_distance_width = halfwidth + other_halfwidth;
				const auto max_distance_height = halfheight + other_halfheight;
				if (std::abs(car_x - other_car_x) < max_distance_width
					&& std::abs(car_y - other_car_y) < max_distance_height)
					result.emplace_back(i, j);
			}
			else
			{
				auto found_intersection = false;

				if (car_is_circular && !other_car_is_circular)
				{
					std::swap(other_car_x, car_x);
					std::swap(other_car_y, car_y);
					std::swap(other_halfwidth, halfwidth);
					std::swap(other_halfheight, halfheight);
					std::swap(other_car_is_circular, car_is_circular);
				}

				// Step 1: point-in-circle test of corners
				const auto corners = std::vector<std::pair<double, double>>{
				{ car_x - halfwidth, car_y - halfheight },

				{ car_x - halfwidth, car_y + halfheight },
				{ car_x + halfwidth, car_y - halfheight },
				{ car_x + halfwidth, car_y + halfheight },
				};

				for (const auto [corner_x, corner_y] : corners)
				{
					if (sqr(corner_x - other_car_x) + sqr(corner_y - other_car_y)
						< sqr(other_halfwidth))
					{
						result.emplace_back(i, j);
						found_intersection = true;
						break;
					}
				}

				if (found_intersection)
					continue;

				// Step 2: point-in-rectangle test of extremes of circle
				const auto extremes = std::vector<std::pair<double, double>>{
				{ other_car_x - other_halfwidth, other_car_y },
				{ other_car_x + other_halfwidth, other_car_y },
				{ other_car_x, other_car_y - other_halfheight },
				{ other_car_x, other_car_y + other_halfheight },

				// Add center-point, which detects fully overlapping shapes.
				// In case the sizes are equivalent, the extremes do not fall
				// inside the rectangle.
				{ other_car_x, other_car_y },
				};

				for (const auto [extrema_x, extrema_y] : extremes)
				{
					if (car_x - halfwidth < extrema_x < car_x + halfwidth
						&& car_y - halfheight < extrema_y < car_y + halfheight)
					{
						result.emplace_back(i, j);
						break;
					}
				}
			}
		}
	}

	return result;
}
#pragma endregion
#endif // Shared_Code