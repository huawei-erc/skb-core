book(potter, rowling).
book(karenina, tolstoy).
book(elements, euclid).
genre(potter, fiction).
genre(karenina, classic).
genre(elements, science).

author(Author, Genre) :- (book(X, Author), genre(X, Genre)).





