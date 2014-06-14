/*
 * Alarm.h
 *
 *  Created on: 21. 5. 2014
 *      Author: Vladimír Matěna
 *
 *  CDEECO++ component monitoring temperature provided by Thermometers
 *
 */

#ifndef ALARM_H
#define ALARM_H

#include <array>
#include <algorithm>
#include <climits>

#include "cdeeco/Component.h"

namespace Alarm {
	/**
	 * Portable thermometer knowledge
	 */
	struct Knowledge: CDEECO::Knowledge {
		struct Position {
			int lat;
			int lon;
		} position;

		static const CDEECO::Id NO_MEMBER = ULONG_MAX;
		struct SensorInfo {
			CDEECO::Id id;
			PortableSensor::Knowledge::Value value;
			PortableSensor::Knowledge::Position position;
		};

		typedef std::array<SensorInfo, 10> SensorData;
		SensorData nearbySensors;

		bool tempCritical;
	};

	/**
	 * Temperature check task
	 */
	class Check: public CDEECO::PeriodicTask<Knowledge, bool> {
	public:
		Check(auto &component, auto &out) :
				PeriodicTask(3000, component, out) {
		}

	private:
		bool run(const Knowledge in) {
			console.print(TaskInfo, "Alarm check task\n");
			for(auto info : in.nearbySensors) {
				if(info.id != Knowledge::NO_MEMBER) {
					console.print(TaskInfo, "> Id: %x", info.id);

					console.print(TaskInfo, "\tTemp: ");
					console.printFloat(TaskInfo, info.value.temperature, 2);
					console.print(TaskInfo, "°C");

					console.print(TaskInfo, "\tHumi: ");
					console.printFloat(TaskInfo, info.value.humidity, 2);
					console.print(TaskInfo, "%%");

					console.print(TaskInfo, "\tPos: ");
					console.printFloat(TaskInfo, info.position.lat, 6);
					console.print(TaskInfo, " ");
					console.printFloat(TaskInfo, info.position.lon, 6);

					console.print(TaskInfo, "\n");
				}
			}
			console.print(TaskInfo, "\n");

			// Check temperatures for dangerous conditions
			const float threshold = 26.0f;

			for(auto info : in.nearbySensors)
				if(info.value.temperature > threshold)
					return true;
			return false;
		}
	};

	/**
	 * Temperature critical trigger task
	 */
	class Critical: public CDEECO::TriggeredTask<Knowledge, bool, void> {
	public:
		Critical(auto &component, auto &trigger) :
				TriggeredTask(trigger, component) {
		}
	protected:
		void run(const Knowledge in) {
			if(in.tempCritical) {
				console.print(TaskInfo, "##############################################################\n");
				console.print(TaskInfo, "# Critical task triggered on change and temp is CRITICAL !!! #\n");
				console.print(TaskInfo, "##############################################################\n");
			}
		}
	};

	/**
	 * PortableThermometer component class
	 */
	class Component: public CDEECO::Component<Knowledge> {
	public:
		static const CDEECO::Type Type = 0x00000002;
		Check check = Check(*this, this->knowledge.tempCritical);
		Critical critical = Critical(*this, this->knowledge.tempCritical);

		Component(auto &system, const CDEECO::Id id) :
				CDEECO::Component<Knowledge>(id, Type, system) {
			// Initialize knowledge
			memset(&knowledge, 0, sizeof(Knowledge));
			knowledge.nearbySensors.fill( { Knowledge::NO_MEMBER, { 0, 0 } });
		}
	};
}

#endif // ALARM_H
