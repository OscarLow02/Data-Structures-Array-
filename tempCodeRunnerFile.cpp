void analyzeFakePoliticalNews(NewsArray& newsArray) {
        string months[12] = {
            "January", "February", "March", "April", "May", "June",
            "July", "August", "September", "October", "November", "December"
        };
        
        int fakeCount[12] = {0}; // Fake news count for each month
        int totalCount[12] = {0}; // Total news count (Fake + True) for each month
    
        // Step 1: Search for articles from 2016
        searchArticlesByYear(fakeNews, "2016");
        searchArticlesByYear(trueNews, "2016");
    
        // Step 2: Search for political articles in 2016
        searchArticlesByCategory(fakeNews, "politics");
        searchArticlesByCategory(trueNews, "politics");

        // Step 3: Count Fake and True Political News in Each Month
        for (int i = 0; i < fakeNews.getSize(); i++) {
            Article article = fakeNews.getArticle(i);
            if (article.year == "2016" && article.category == "politics") {
                int monthIndex = article.monthIndex - 1; // Convert 1-12 to 0-based index
                fakeCount[monthIndex]++;
            }
        }

        for (int i = 0; i < trueNews.getSize(); i++) {
            Article article = trueNews.getArticle(i);
            if (article.year == "2016" && article.category == "politics") {
                int monthIndex = article.monthIndex - 1;
                totalCount[monthIndex]++;
            }
        }
        // Step 4: Display Results
        cout << "Percentage of Fake Political News Articles in 2016:\n";
        for (int i = 0; i < 12; i++) {
            int fake = fakeCount[i];
            int total = fake + totalCount[i];

            double percentage = (total == 0) ? 0.0 : (static_cast<double>(fake) / total) * 100;
            int numStars = static_cast<int>(percentage); // Each '*' represents 1%

            cout << months[i] << "\t| ";
            for (int j = 0; j < numStars; j++) {
                cout << "*";
            }
            cout << " " << percentage << "%" << endl;
        }
    }