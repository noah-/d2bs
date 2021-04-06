#pragma once
#include "PathReducer.h"
#include "../Path.h"
#include "../../Diablo_II/ActMap.h"

namespace Mapping {
namespace Pathing {
namespace Reducing {

#pragma warning(disable : 4512)

class WalkPathReducer : public PathReducer {
  private:
    ActMap* map;
    Distance distance;
    int range;

    inline void swap(int* x, int* y) {
        int t = *x;
        *x = *y;
        *y = t;
    }

    /*
    void Line(Point start, Point end, PointList& list, bool absolute)
    {
            int x0 = start.first, y0 = start.second,
                    x1 = end.first, y1 = end.second;
            bool steep = abs(y1 - y0) > abs(x1 - x0);
            if(steep)
            {
                    swap(&x0, &y0);
                    swap(&x1, &y1);
            }
            if(x0 > x1)
            {
                    swap(&x0, &x1);
                    swap(&y0, &y1);
            }
            int dx = x1 - x0, dy = abs(y1 - y0),
                    error = dx/2,
                    ystep, y = y0;

            if(y0 < y1) ystep = 1;
            else ystep = -1;

            for(int x = x0; x < x1; x++)
            {
                    Point p((steep ? y : x), (steep ? x : y));
                    list.push_back(p);

                    if(!(map->PathIsWalkable(list, absolute) && distance(start, p) < range))
                    {
                            list.pop_back();
                            break;
                    }

                    error -= dy;
                    if(error < 0)
                    {
                            y += ystep;
                            error += dx;
                    }
            }
    }
    */

  public:
    WalkPathReducer(const WalkPathReducer&);
    WalkPathReducer& operator=(const WalkPathReducer&);
    WalkPathReducer(ActMap* m, Distance d, int _range = 20) : map(m), distance(d), range(_range * 10) {
    }

    /*
    void Reduce(PointList const & in, PointList& out, bool abs)
    {
            PointList::const_iterator it = in.begin(), end = in.end();
            out.push_back(*it);
            while(it != end)
            {
                    PointList path;
                    Point current = *(out.end()-1);
                    Point last;
                    do {
                            // doesn't matter how far we are, break out here
                            if(it == end) break;

                            last = *(it++);
                            path.clear();
                            Line(current, last, path, abs);
                    } while(map->PathIsWalkable(path, abs) && distance(current, last) < range);
                    it--;
                    out.push_back(*it);
                    it++;
            }
    }
    */

/*function getDistance(a, b) {
    return Math.hypot(a.x - b.x, a.y - b.y);
}

function getSlope(a, b) {
    return (a.y - b.y) / (a.x - b.x);
}*/
    void Reduce(PointList const& in, PointList& out, bool abs) {
        if (in.size() < 2) {
            out = in;
            return;
        }

        PointList::const_iterator it = in.begin();

        bool init = true;
        int slope = 0;
        Point next;
        Point last;
        Point first;
        first = *it;

        for (; *it != in.back(); it++) {
            next = *it;
            int slope_next = Slope(first, next);
            if (init || slope_next != slope) {
                init = false;
                out.push_back(first);
                last = first;
                slope = slope_next;
            } else if (Euclidean(last, next) >= range) {
                out.push_back(first);
                last = first;
            }
            first = next;
        }

        out.push_back(in.back());
        /*PointList::const_iterator it = in.begin(), end = in.end();
        out.push_back(*it);
        Point lineStartStep;
        Point lineStartPoint;
        Point currentStep;
        Point previousPoint;
        Point secondStep;
        int differentStepCount = 0;
        int lineLength = 0;
        bool first = true;
        while (it != end) {
            if (first) {
                lineStartPoint = *it;
                it++;
                lineLength++;
                first = false;
                if (it != end) {
                    lineStartStep = Point(lineStartPoint.first - (*it).first, lineStartPoint.second - (*it).second);
                    secondStep = lineStartStep;
                }
                continue;
            }
            previousPoint = *it;
            it++;
            lineLength++;
            if (it == end) {
                out.push_back(previousPoint);
                break;
            }
            currentStep = Point(previousPoint.first - (*it).first, previousPoint.second - (*it).second);
            if (currentStep != lineStartStep) {
                if (lineStartStep == secondStep) {
                    secondStep = currentStep;
                }
                differentStepCount++;
            } else if (differentStepCount == 1) {
                differentStepCount = 0;
            }
            if (lineLength > 5 && ((currentStep != lineStartStep && currentStep != secondStep) || differentStepCount > 1 || lineLength > 20)) {
                out.push_back(previousPoint);
                lineStartStep = Point(currentStep);
                secondStep = lineStartStep;
                lineLength = 1;
                differentStepCount = 0;
            }
        }
        /*
        char mapFileName[200];
        sprintf_s(mapFileName, "b:\\maps-pathing\\%03d %s in.txt", map->GetLevel()->dwLevelNo, GetLevelName(map->GetLevel()));
        map->Dump(mapFileName, in);
        sprintf_s(mapFileName, "b:\\maps-pathing\\%03d %s out.txt", map->GetLevel()->dwLevelNo, GetLevelName(map->GetLevel()));
        map->Dump(mapFileName, out);
        */
    }

    bool Reject(Point const& pt, bool abs) {
        return checkFlag(map->SpaceGetData(pt, abs));
        /*return map->SpaceHasFlag(ActMap::Avoid, pt, abs)     ||
                   map->SpaceHasFlag(ActMap::BlockWalk, pt, abs) ||
                   map->SpaceHasFlag(ActMap::BlockPlayer, pt, abs);*/
    }

    void GetOpenNodes(Point const& center, PointList& out, Point const& endpoint) {
        /*for(int i = 1; i >= -1; i--)
        {
                for(int j = 1; j >= -1; j--)
                {
                        if( i == 0 && j == 0)
                                continue;
                        out.push_back(Point(center.first+i, center.second+j));
                }
        }*/

        out.push_back(Point(center.first + 1, center.second));
        out.push_back(Point(center.first - 1, center.second));
        out.push_back(Point(center.first, center.second + 1));
        out.push_back(Point(center.first, center.second - 1));
        out.push_back(Point(center.first + 1, center.second + 1));
        out.push_back(Point(center.first - 1, center.second - 1));
        out.push_back(Point(center.first + 1, center.second - 1));
        out.push_back(Point(center.first - 1, center.second + 1));
    }

    int GetPenalty(Point const& pt, bool abs) {
        if (checkFlag(map->SpaceGetDataWide(pt, abs))) {
            return 50;
        }

        int data = map->SpaceGetData(pt, abs);

        if ((data & ActMap::Object) == ActMap::Object) {
            return 60;
        }

        if ((data & ActMap::ClosedDoor) == ActMap::ClosedDoor) {
            return 80;
        }

        return 0;
        // return map->SpaceHasFlagWide(ActMap::BlockWalk, pt, abs) ? 50 : (data & ActMap::Object == ActMap::Object) ? 60 : (map->SpaceHasFlag(ActMap::ClosedDoor, pt,
        // abs) ? 80 : 0));
    }

    void MutatePoint(Point& pt, bool absv) {
        bool mutated = false;
        // find the nearest walkable space
        int area[7][7];

        for (int i = -3; i <= 3; i++) {
            for (int j = -3; j <= 3; j++) {
                if (i == 0 && j == 0 || (abs(i) + abs(j)) == 6)
                    continue;
                Point ptN(pt.first + i, pt.second + j);
                area[3 + i][3 + j] = map->GetMapData(ptN, absv);
            }
        }

        for (int i = -2; i <= 2; i++) {
            for (int j = -2; j <= 2; j++) {
                if (i == 0 && j == 0 || (abs(i + j)) == 1)
                    continue;

                if (!checkFlag(area[3 + i][3 + j] | area[3 + i + 1][3 + j] | area[3 + i - 1][3 + j] | area[3 + i][3 + j + 1] | area[3 + i][3 + j - 1])) {
                    pt.first += i;
                    pt.second += j;
                    mutated = true;
                    return;
                } else {
                    j++;
                }
            }
        }
    }
};

#pragma warning(default : 4512)

} // namespace Reducing
} // namespace Pathing
} // namespace Mapping
