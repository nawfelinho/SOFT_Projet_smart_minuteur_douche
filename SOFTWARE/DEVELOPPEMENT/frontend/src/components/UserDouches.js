"use client"

import { useEffect, useState } from "react"
import { useParams, useNavigate } from "react-router-dom"
import { Chart as ChartJS, LineElement, PointElement, LinearScale, CategoryScale, Legend, Tooltip } from "chart.js"
import { Line } from "react-chartjs-2"

ChartJS.register(LineElement, PointElement, LinearScale, CategoryScale, Legend, Tooltip)

function UserDouches() {
    const { id } = useParams()
    const navigate = useNavigate()
    const [douches, setDouches] = useState([])
    const [userName, setUserName] = useState("")
    const [userStats, setUserStats] = useState({ total: 0, moyenne: 0 })

    useEffect(() => {
        fetch(`http://localhost:8080/users/${id}`)
            .then((res) => res.json())
            .then((user) => setUserName(`${user.prenom} ${user.nom}`))

        fetch("http://localhost:8080/douches")
            .then((res) => res.json())
            .then((data) => {
                const filtered = data.filter((d) => d.user?.id === Number.parseInt(id))
                setDouches(filtered)
                const total = filtered.reduce((sum, d) => sum + d.duree, 0)
                const moyenne = filtered.length ? Math.floor(total / filtered.length) : 0
                setUserStats({ total, moyenne })
            })
            .catch((err) => console.error("Erreur chargement douches", err))
    }, [id])

    const formatTemps = (seconds) => {
        const m = Math.floor(seconds / 60)
        const s = seconds % 60
        return `${m}min ${s < 10 ? "0" : ""}${s}s`
    }

    const moyenneGlissante = (data, windowSize = 3) => {
        const res = []
        for (let i = 0; i < data.length; i++) {
            const start = Math.max(0, i - windowSize + 1)
            const window = data.slice(start, i + 1)
            const moyenne = window.reduce((a, b) => a + b, 0) / window.length
            res.push(Math.round(moyenne))
        }
        return res
    }

    const DUREE_OBJECTIF = 300 // 5 min = 300s

    function objectifProchaineDouche() {
        if (douches.length === 0) {
            return (
                <div className="objective-card">
                    <p style={{ marginBottom: 4, fontSize: "1rem" }}>
                        <b>
                            Le temps moyen d'une douche recommandé par l'OMS (Organisation mondiale de la santé) est d'environ 5
                            minutes.
                        </b>
                    </p>
                    <p style={{ margin: 0 }}>
                        <b>Objectif de temps de la prochaine douche :</b>
                        <br />
                        Vous n'avez pas encore démarré de douches, essayez de prendre de bonnes habitudes ! Pour votre première
                        douche, essayez d'atteindre l'objectif suivant : <b>5 min</b>
                    </p>
                </div>
            )
        }
        const moyenne = userStats.moyenne
        if (moyenne <= DUREE_OBJECTIF) {
            return (
                <div className="objective-card">
                    <p style={{ marginBottom: 4, fontSize: "1rem" }}>
                        <b>
                            Le temps moyen d'une douche recommandé par l'OMS (Organisation mondiale de la santé) est d'environ 5
                            minutes.
                        </b>
                    </p>
                    <p style={{ margin: 0 }}>
                        <b>Objectif de temps de la prochaine douche :</b>
                        <br />
                        Super, vous avez atteint l'objectif recommandé, gardez vos bonnes habitudes ! Pour la prochaine douche,
                        essayez d'atteindre l'objectif suivant : <b>5 min</b>
                    </p>
                </div>
            )
        }
        // Utilisateur au-dessus de 5 min
        const last3 = douches.slice(-3).map((d) => d.duree)
        const moy3 = last3.length ? last3.reduce((a, b) => a + b, 0) / last3.length : moyenne
        let objReduit = Math.max(moy3 * 0.9, 300) // -10%, au minimum 5min
        // Limite la chute brutale d'un coup
        const last = douches[douches.length - 1].duree
        if (last - objReduit > 30) objReduit = last - 30
        objReduit = Math.round(objReduit)

        if (objReduit <= 330) {
            return (
                <div className="objective-card">
                    <p style={{ marginBottom: 4, fontSize: "1rem" }}>
                        <b>
                            Le temps moyen d'une douche recommandé par l'OMS (Organisation mondiale de la santé) est d'environ 5
                            minutes.
                        </b>
                    </p>
                    <p style={{ margin: 0 }}>
                        <b>Objectif de temps de la prochaine douche :</b>
                        <br />
                        Bravo, vous vous rapprochez de l'objectif ! Essayez d'atteindre ou de maintenir 5 minutes à chaque douche.
                    </p>
                </div>
            )
        }
        return (
            <div className="objective-card">
                <p style={{ marginBottom: 4, fontSize: "1rem" }}>
                    <b>
                        Le temps moyen d'une douche recommandé par l'OMS (Organisation mondiale de la santé) est d'environ 5
                        minutes.
                    </b>
                </p>
                <p style={{ margin: 0 }}>
                    <b>Objectif de temps de la prochaine douche :</b>
                    <br />
                    Vous êtes sur la bonne voie ! Pour votre prochaine douche, essayez d'atteindre l'objectif suivant :{" "}
                    <b>{formatTemps(objReduit)}</b>
                </p>
            </div>
        )
    }

    // Graphiques
    const durees = douches.map((d) => d.duree)
    const labels = douches.map((_, i) => `Douche ${i + 1}`)
    const moyennes = moyenneGlissante(durees)

    const chartData = {
        labels,
        datasets: [
            {
                label: "Durée des douches (s)",
                data: durees,
                borderColor: "#667eea",
                backgroundColor: "#667eea",
                fill: false,
                tension: 0.3,
                pointRadius: 4,
                pointHoverRadius: 6,
            },
            {
                label: "Temps de douche moyen (moyenne glissante)",
                data: moyennes,
                borderColor: "#f6ad55",
                borderDash: [5, 5],
                fill: false,
                tension: 0.3,
                pointRadius: 0,
            },
        ],
    }

    const chartOptions = {
        responsive: true,
        maintainAspectRatio: false,
        plugins: {
            legend: { position: "top" },
        },
        scales: {
            y: { beginAtZero: true, title: { display: true, text: "Durée (s)" } },
        },
    }

    return (
        <div className="container fade-in">
            <button className="nav-button" onClick={() => navigate("/")}>
                ← Choisir un autre utilisateur
            </button>

            <div className="card">
                <h1>🚿 Douches de {userName}</h1>

                <div className="stats-grid">
                    <div className="stat-card">
                        <span className="stat-value">{douches.length}</span>
                        <span className="stat-label">Douches prises</span>
                    </div>
                    <div className="stat-card">
                        <span className="stat-value">{formatTemps(userStats.total)}</span>
                        <span className="stat-label">Temps total</span>
                    </div>
                    <div className="stat-card">
                        <span className="stat-value">{formatTemps(userStats.moyenne)}</span>
                        <span className="stat-label">Temps moyen</span>
                    </div>
                </div>

                {objectifProchaineDouche()}

                <h2>
          <span role="img" aria-label="courbe">
            📈
          </span>{" "}
                    Évolution des durées de douche
                </h2>
                <div className="chart-container">
                    <div style={{ width: "100%", height: "400px" }}>
                        <Line data={chartData} options={chartOptions} />
                    </div>
                </div>

                <div className="tips-section">
                    <h2 style={{ display: "flex", alignItems: "center" }}>
            <span role="img" aria-label="Conseils" style={{ marginRight: "0.5rem" }}>
              💡
            </span>
                        Conseils utiles
                    </h2>
                    <ul className="tips-list">
                        <li>🛁 Une douche de 5 minutes consomme environ 60 litres d'eau.</li>
                        <li>💰 L'eau chaude représente 20% de la consommation énergétique d'un foyer.</li>
                        <li>🌿 Réduire la durée de douche permet d'économiser jusqu'à 270 €/an.</li>
                        <li>🏷️ Un bon objectif est une douche de 5 minutes (300 s).</li>
                        <li>📈 Surveillez votre moyenne pour progresser durablement !</li>
                    </ul>
                </div>

                <h2>🔮 Prévisions</h2>
                {douches.length < 5 ? (
                    <div className="alert alert-info">
            <span role="img" aria-label="lock" style={{ marginRight: "0.5rem" }}>
              🔒
            </span>
                        Faites au moins cinq douches pour débloquer les prévisions.
                    </div>
                ) : (
                    <button className="btn btn-large" onClick={() => navigate(`/user/${id}/previsions`)}>
                        Accéder aux prévisions
                    </button>
                )}

                <h2>📊 Données détaillées des douches</h2>
                <div className="table-container">
                    <table className="table">
                        <thead>
                        <tr>
                            <th>Début</th>
                            <th>Fin</th>
                            <th>Durée</th>
                            <th>Dépassement</th>
                        </tr>
                        </thead>
                        <tbody>
                        {douches.map((d, index) => (
                            <tr key={index}>
                                <td>{new Date(d.dateDebut).toLocaleString()}</td>
                                <td>{new Date(d.dateFin).toLocaleString()}</td>
                                <td>{formatTemps(d.duree)}</td>
                                <td>{d.tempsDepasse > 0 ? formatTemps(d.tempsDepasse) : "-"}</td>
                            </tr>
                        ))}
                        </tbody>
                    </table>
                </div>
            </div>
        </div>
    )
}

export default UserDouches
