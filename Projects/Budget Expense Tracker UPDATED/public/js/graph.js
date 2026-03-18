// Declare the chart dimensions and margins.
const margin = { top: 30, right: 30, bottom: 40, left: 70 };
const width = 900 - margin.left - margin.right;
const height = 500 - margin.top - margin.bottom;

// Declare the x & y scales.
const x = d3.scaleTime().range([0, width]);
const y = d3.scaleLinear().range([height, 0]);

// Create the SVG element & append to graph container
const svg = d3.select("#graph-container")
    .append("svg")
        .attr("width", width + margin.left + margin.right)
        .attr("height", height + margin.top + margin.bottom)
    .append("g")
        .attr("transform", `translate(${margin.left},${margin.top})`);

// Fetch live finlines data from the API
fetch('/api/finlines')
    .then(res => res.json())
    .then(data => {
        if (!data || data.length === 0) return;

        const dataset = data.map(d => ({
            date: new Date(d.LineDate),
            value: parseFloat(d.LineBalance)
        })).filter(d => !isNaN(d.value));

        if (dataset.length === 0) return;

        x.domain(d3.extent(dataset, d => d.date));
        y.domain([0, d3.max(dataset, d => d.value)]);

        // Add x-axis
        svg.append("g")
            .attr("transform", `translate(0,${height})`)
            .call(d3.axisBottom(x)
                .ticks(d3.timeMonth.every(1))
                .tickFormat(d3.timeFormat("%b %Y")));

        // Add y-axis
        svg.append("g")
            .call(d3.axisLeft(y));

        // Create line generator
        const line = d3.line()
            .x(d => x(d.date))
            .y(d => y(d.value));

        // Draw the line
        svg.append("path")
            .datum(dataset)
            .attr("fill", "none")
            .attr("stroke", "steelblue")
            .attr("stroke-width", 1.5)
            .attr("d", line);
    })
    .catch(err => console.error('Failed to load graph data:', err));
