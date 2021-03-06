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

package Job;

import Game.Pawn;
import Map.MapCoordinate;
import PathFinding.PathFinding;


/**
 *
 * @author Impaler
 */
public class WanderJob extends Job {
	
	PathFinding Pathing;
	
	public WanderJob() {
		super();
		Name = "Wandering Aimlessly";

		Pathing = PathFinding.getSinglton();
		Type = Job.JobType.JOB_WANDER;
	}

	/*
	@Override
	public boolean addPawn(Pawn NewPawn) {
		if (super.addPawn(NewPawn)) {
			Task newTask = nextTask(NewPawn);
			NewPawn.setTask(newTask);
			return true;
		} else {
			return false;
		}
	}*/

	public Task nextTask(Pawn IdlePawn) {
		MapCoordinate Destination = RandomDestination(IdlePawn);
		if (Destination != null) {
			return new Task(this, TaskType.TASK_GOTO, Destination);
		} else {
			releaseCitizen(IdlePawn);
			return new Task(this, TaskType.TASK_TRAPPED, null);
		}
	}

	public boolean needsWorkers() {
		return true;
	}

	public float EvaluatePawn(Pawn CandidateCitizen) {
		float Evaluation = 0;
		if (Workers.contains(CandidateCitizen))
			Evaluation += 1;
		return Evaluation + 1;
	}

	public MapCoordinate RandomDestination(Pawn  Wanderer) {
		MapCoordinate StartLocation = Wanderer.getLocation();
		if (Pathing.getDirectionFlags(StartLocation, Wanderer.getMovementModality()).cardinality() != 0) {//Is the current location impassable
			MapCoordinate DestinationCoordinates = Pathing.Tester.getRandomPassableCoordinate();  // This needs to get DIFFERENT coords each time

			while (!Wanderer.isDestinationReachable(DestinationCoordinates)) {
				DestinationCoordinates = Pathing.Tester.getRandomPassableCoordinate();
			}
			return DestinationCoordinates;
		}
		return null;
	}
}
