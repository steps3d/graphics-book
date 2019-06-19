a = []
q = []
q.append ( a.vertices )		# add vertices of A
q.append ( b.vertices )		# add vertices of B
q.sort ( compareX )			# sort by smallest x coordinate

while not q.empty():		# while we have an event
	e = q.pop ()			# pop edge with smallest x-coordinate
	a.append ( e )			# add this edge to active edge table
	a.sort ( compareY )	
							# check whether we have to remove edge
	if a.contains ( e.begin ) and a.contains ( e.end ):
		a.remove ( e )
							# find edge intersections
	q1 = computeEdgeCrossings ( a )
							# merge sort them into q
	q.mergeSort ( q1, compareX )
							# perform boolean operation on a
	computeBoolean ( operation, a, e )

	