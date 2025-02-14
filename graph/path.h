//Copyright 2017 Ryan Wick

//This file is part of Bandage

//Bandage is free software: you can redistribute it and/or modify
//it under the terms of the GNU General Public License as published by
//the Free Software Foundation, either version 3 of the License, or
//(at your option) any later version.

//Bandage is distributed in the hope that it will be useful,
//but WITHOUT ANY WARRANTY; without even the implied warranty of
//MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//GNU General Public License for more details.

//You should have received a copy of the GNU General Public License
//along with Bandage.  If not, see <http://www.gnu.org/licenses/>.


#ifndef PATH_H
#define PATH_H

#include "graphlocation.h"

#include <QByteArray>
#include <QList>
#include <QString>
#include <QStringList>

#include <vector>

class DeBruijnNode;
class DeBruijnEdge;
class AssemblyGraph;

class Path
{
public:
    //CREATORS
    Path() {}
    Path(GraphLocation location);
    static Path makeFromUnorderedNodes(QList<DeBruijnNode *> nodes,
                                       bool strandSpecific);
    static Path makeFromUnorderedNodes(const std::vector<DeBruijnNode *>& nodes,
                                       bool strandSpecific);
    static Path makeFromOrderedNodes(QList<DeBruijnNode *> nodes,
                                     bool circular);
    static Path makeFromString(const QString& pathString,
                               const AssemblyGraph &graph,
                               bool circular,
                               QString * pathStringFailure);

    //ACCESSORS
    const QList<DeBruijnNode *>& getNodes() const {return m_nodes;}
    const QList<DeBruijnEdge *>& getEdges() const {return m_edges;}
    bool isEmpty() const {return m_nodes.empty();}
    bool isCircular() const;
    bool haveSameNodes(const Path& other) const;
    bool hasNodeSubset(const Path& other) const;
    QByteArray getPathSequence() const;
    QString getFasta() const;
    QString getString(bool spaces) const;
    int getLength() const;
    QList<Path> extendPathInAllPossibleWays() const;
    bool canNodeFitOnEnd(DeBruijnNode * node, Path * extendedPath) const;
    bool canNodeFitAtStart(DeBruijnNode * node, Path * extendedPath) const;

    bool containsNode(const DeBruijnNode * node) const;
    bool containsEntireNode(const DeBruijnNode * node) const;
    bool isInMiddleOfPath(const DeBruijnNode * node) const;
    unsigned int numberOfOccurrencesInMiddleOfPath(const DeBruijnNode * node) const;
    bool isStartingNode(const DeBruijnNode * node) const;
    bool isEndingNode(const DeBruijnNode * node) const;
    double getStartFraction() const;
    double getEndFraction() const;
    size_t getNodeCount() const { return m_nodes.size(); }
    [[nodiscard]] GraphLocation getStartLocation() const {return m_startLocation;}
    [[nodiscard]] GraphLocation getEndLocation() const {return m_endLocation;}
    bool operator==(Path const &other) const;

    //MODIFERS
    bool addNode(DeBruijnNode * newNode, bool strandSpecific, bool makeCircularIfPossible);
    void extendPathToIncludeEntirityOfNodes();

    //STATIC
    static QList<Path> getAllPossiblePaths(GraphLocation startLocation,
                                           GraphLocation endLocation,
                                           int nodeSearchDepth,
                                           int minDistance, int maxDistance);

private:
    GraphLocation m_startLocation;
    GraphLocation m_endLocation;
    QList<DeBruijnNode *> m_nodes;
    QList<DeBruijnEdge *> m_edges;

    void buildUnambiguousPathFromNodes(QList<DeBruijnNode *> nodes,
                                       bool strandSpecific);
    static QByteArray modifySequenceUsingOverlap(QByteArray sequence, int overlap) ;
    bool checkForOtherEdges();
};

#endif // PATH_H
