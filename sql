select tracks.title, 
	artists.artistid as artistid,
	artists.name as artistname, 
	CASE WHEN albumartists.artistid IS NOT NULL
	THEN albumartists.artistid 
	ELSE artists.artistid
	END as albumartistid,
	CASE WHEN albumartists.name  IS NOT NULL
	THEN albumartists.name
	ELSE artists.name
	END as albumartistname
FROM tracks inner join
artists ON tracks.artistid = artists.artistid
left join artists albumartists ON tracks.albumartistid = albumartists.artistid
order by albumartistid;
