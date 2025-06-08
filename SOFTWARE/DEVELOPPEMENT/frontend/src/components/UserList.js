"use client"

import { useEffect, useState } from "react"
import { useNavigate } from "react-router-dom"

function UserList() {
    const [users, setUsers] = useState([])
    const [nom, setNom] = useState("")
    const [prenom, setPrenom] = useState("")
    const [message, setMessage] = useState("")
    const navigate = useNavigate()

    useEffect(() => {
        fetch("http://localhost:8080/users")
            .then((res) => res.json())
            .then((data) => setUsers(data.filter((u) => u !== null)))
            .catch((err) => console.error("Erreur de chargement des utilisateurs :", err))
    }, [])

    const handleUserClick = (user) => {
        // Envoi du nom au backend BLE
        fetch("http://localhost:8080/ble/sendUserToEsp32", {
            method: "POST",
            headers: { "Content-Type": "application/json" },
            body: JSON.stringify({ username: user.nom }),
        })
            .then((res) => res.json())
            .then(() => {
                navigate(`/user/${user.id}`)
            })
            .catch((err) => console.error("Erreur envoi ESP32 :", err))
    }

    const handleCreateUser = () => {
        const trimmedNom = nom.trim()
        const trimmedPrenom = prenom.trim()
        if (trimmedNom.length === 0 || trimmedPrenom.length === 0) {
            setMessage("Champs vides non autorisés")
            return
        }

        if (trimmedNom.length > 20 || trimmedPrenom.length > 20) {
            setMessage("Nom et prénom doivent contenir 20 caractères max")
            return
        }

        const exists = users.some(
            (u) =>
                u && u.nom.toLowerCase() === trimmedNom.toLowerCase() && u.prenom.toLowerCase() === trimmedPrenom.toLowerCase(),
        )

        if (exists) {
            setMessage("Cet utilisateur existe déjà")
            return
        }

        fetch("http://localhost:8080/users", {
            method: "POST",
            headers: { "Content-Type": "application/json" },
            body: JSON.stringify({ nom: trimmedNom.toUpperCase(), prenom: trimmedPrenom }),
        })
            .then((res) => res.json())
            .then(() => {
                setNom("")
                setPrenom("")
                setMessage("Utilisateur créé !")
                return fetch("http://localhost:8080/users")
            })
            .then((res) => res.json())
            .then((data) => setUsers(data.filter((u) => u !== null)))
            .catch((err) => setMessage("Erreur création utilisateur"))
    }

    return (
        <div className="container fade-in">
            <div className="app-header">
                <h1 className="app-title">🚿 Smart Minuteur Douche</h1>
                <p className="app-subtitle">Surveillez et optimisez votre consommation d'eau</p>
            </div>

            <div className="card">
                <h2>👥 Choisissez votre profil</h2>
                <p style={{ color: "#718096", marginBottom: "2rem" }}>
                    Sélectionnez votre profil pour commencer à suivre vos douches
                </p>

                <div className="users-grid">
                    {users.map((user) => (
                        <div key={user.id} className="user-card" onClick={() => handleUserClick(user)}>
                            <div style={{ fontSize: "2rem", marginBottom: "0.5rem" }}>👤</div>
                            <div style={{ fontWeight: "600", fontSize: "1.1rem" }}>{user.prenom}</div>
                            <div style={{ color: "#718096", fontSize: "0.9rem" }}>{user.nom}</div>
                        </div>
                    ))}
                </div>
            </div>

            <div className="card">
                <h3>➕ Créer un nouveau profil</h3>
                <div className="form-inline">
                    <input
                        type="text"
                        className="form-input"
                        placeholder="Prénom"
                        value={prenom}
                        onChange={(e) => setPrenom(e.target.value)}
                        maxLength="20"
                        style={{ flex: 1 }}
                    />
                    <input
                        type="text"
                        className="form-input"
                        placeholder="Nom"
                        value={nom}
                        onChange={(e) => setNom(e.target.value)}
                        maxLength="20"
                        style={{ flex: 1 }}
                    />
                    <button className="btn btn-secondary" onClick={handleCreateUser}>
                        Créer
                    </button>
                </div>

                {message && (
                    <div className={`alert ${message.includes("créé") ? "alert-success" : "alert-error"}`}>{message}</div>
                )}
            </div>
        </div>
    )
}

export default UserList
