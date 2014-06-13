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
			Console::print(TaskInfo, "\n\n\n>>>> Running alarm temp check\n");
			Console::print(TaskInfo, ">>>> Registered sensor and values:\n");
			for(auto info : in.nearbySensors) {
				if(info.id != Knowledge::NO_MEMBER) {
					Console::print(TaskInfo, ">>>>>> Id: %x ", info.id);

					Console::print(TaskInfo, "Temp: ");
					Console::printFloat(TaskInfo, info.value.temperature, 2);
					Console::print(TaskInfo, "°C ");

					Console::print(TaskInfo, "Humi: ");
					Console::printFloat(TaskInfo, info.value.humidity, 2);
					Console::print(TaskInfo, "%% ");

					Console::print(TaskInfo, "Posi: ");
					Console::printFloat(TaskInfo, info.position.lat, 6);
					Console::print(TaskInfo, " ");
					Console::printFloat(TaskInfo, info.position.lon, 6);

					Console::print(TaskInfo, "\n");
				}
			}
			Console::print(TaskInfo, "\n\n\n");

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
				Console::print(TaskInfo, "##############################################################\n");
				Console::print(TaskInfo, "# Critical task triggered on change and temp is CRITICAL !!! #\n");
				Console::print(TaskInfo, "##############################################################\n");
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
