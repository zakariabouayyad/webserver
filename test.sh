BASE_URL="http://localhost/VIKINGS.S4E12.1080p.mp4"

for i in {0..5}; do
    curl "$BASE_URL" > "savings/save${i}.mp4"
done

