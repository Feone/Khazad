/* Copyright 2010 Kenneth 'Impaler' Ferland

This file is part of Khazad.

Khazad is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Khazad is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Khazad.  If not, see <http://www.gnu.org/licenses/> */

package PathFinding;

import Map.MapCoordinate;
import Map.Direction;

/**
 * A simple Node used by the AStar pathing class, it holds data placed in it 
 * durring the Node expansion method, the primary data points are the parent node
 * (used to reconstruct the final path), the cumulative edge cost of the parent
 * chain and the minimum distance to reach the goal.
 * 
 * @author Impaler
 */
public class AStarNode implements Comparable {

    float TotalCost;
    float TieBreakerValue;

	MapCoordinate LocationCoordinates;

    float PathLengthFromStart;
    float MinimumCostToGoal;

    AStarNode Parent;
    Direction ParentDirection;
	
    AStarNode() {
	
	}

    void Set(MapCoordinate TargetCoordinates, AStarNode ParentNode, Direction SourceDirection, float DistanceFromStart, float MinimumCost, float TieBreaker) {
        Parent = ParentNode;
        ParentDirection = SourceDirection;
        LocationCoordinates = TargetCoordinates;
        PathLengthFromStart = DistanceFromStart;
        MinimumCostToGoal = MinimumCost;
        TieBreakerValue = TieBreaker;
        TotalCost = PathLengthFromStart + MinimumCostToGoal;
    }

	public int compareTo(Object TargetObject) {
		//if (TargetObject instanceof AStarNode) {
			AStarNode TargetNode = (AStarNode) TargetObject;
			if (TotalCost < TargetNode.TotalCost) {
				return -1;
			} else if (TotalCost == TargetNode.TotalCost) {
				if (TieBreakerValue < TargetNode.TieBreakerValue) {
					return -1;
				} else {
					return 1;
				}
			} else {
				return 1;
			}
		//}
		//return 0;
	}

    boolean equals(AStarNode other) {
        return LocationCoordinates.equals(other.LocationCoordinates);
    }

    MapCoordinate getCoordinates() {
        return LocationCoordinates;
    }
}
