import React from 'react';
import { Line } from 'react-chartjs-2';
import {
    Chart as ChartJS,
    CategoryScale,
    LinearScale,
    PointElement,
    LineElement,
    Title,
    Tooltip,
    Legend,
} from 'chart.js';

ChartJS.register(CategoryScale, LinearScale, PointElement, LineElement, Title, Tooltip, Legend);

function DouchesChart({ douches }) {
    const data = {
        labels: douches.map((_, index) => `Douche ${index + 1}`),
        datasets: [
            {
                label: 'Durée (secondes)',
                data: douches.map(d => d.duree),
                borderColor: 'blue',
                backgroundColor: 'lightblue',
                tension: 0.3
            },
        ],
    };

    const options = {
        responsive: true,
        plugins: {
            legend: {
                position: 'top',
            },
            title: {
                display: true,
                text: 'Évolution des durées de douche',
            },
        },
    };

    return <Line data={data} options={options} />;
}

export default DouchesChart;
