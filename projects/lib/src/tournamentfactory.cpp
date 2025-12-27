/*
    This file is part of Cute Chess.
    Copyright (C) 2008-2018 Cute Chess authors

    Cute Chess is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Cute Chess is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Cute Chess.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "tournamentfactory.h"
#include "roundrobintournament.h"
#include "gauntlettournament.h"
#include "knockouttournament.h"
#include "pyramidtournament.h"

Tournament* TournamentFactory::create(const QString& type,
				      GameManager* manager,
				      QObject* parent)
{
	if (type == "round-robin")
		return new RoundRobinTournament(manager, parent);
	if (type == "gauntlet")
		return new GauntletTournament(manager, parent);
	if (type == "knockout")
		return new KnockoutTournament(manager, parent);
	if (type == "pyramid")
		return new PyramidTournament(manager, parent);

	return nullptr;
}

Tournament* TournamentFactory::load(const QJsonObject& json, GameManager* gameManager, QObject* parent)
{
    QString type = json["type"].toString();
    Tournament* tournament = create(type, gameManager, parent);

    if (!tournament) {
        qWarning() << "TournamentFactory: JSON missing 'type' field.";
        return nullptr;
    }

    // 3. Populate the instance with data
    if (!tournament->loadFromJson(json)) {
        qWarning() << "TournamentFactory: Failed to deserialize tournament data.";
        delete tournament;
        return nullptr;
    }

    return tournament;
}

Tournament* TournamentFactory::loadFromFile(const QString& filePath, GameManager* gameManager, QObject* parent)
{
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        qWarning() << "TournamentFactory: Could not open file " << filePath;
        return nullptr;
    }

    QByteArray data = file.readAll();
    QJsonDocument doc = QJsonDocument::fromJson(data);

    if (doc.isNull() || !doc.isObject()) {
        qWarning() << "TournamentFactory: Invalid JSON in file " << filePath;
        return nullptr;
    }

    return load(doc.object(), gameManager, parent);
}

void TournamentFactory::storeToFile(const QString& filePath, Tournament* tournament)
{
    if (!tournament) {
        qWarning() << "TournamentFactory: Cannot store a null tournament.";
        return;
    }

    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly)) {
        qWarning() << "TournamentFactory: Could not open file for writing: " << filePath;
        return;
    }

    QJsonObject json = tournament->toJson(); 
    
    QJsonDocument doc(json);

    if (file.write(doc.toJson()) == -1) {
        qWarning() << "TournamentFactory: Failed to write data to " << filePath;
    }

    file.close();
}