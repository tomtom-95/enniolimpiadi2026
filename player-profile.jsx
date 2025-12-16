import React, { useState } from 'react';
import { Trophy, Medal, Target, TrendingUp, Calendar, Users, ChevronRight, Star, Flame, Award } from 'lucide-react';

// Mock player data - in real app this would come from your backend
const playerData = {
  id: 1,
  name: "Marco Rossi",
  nickname: "Il Fenomeno",
  avatar: null,
  joinedDate: "2024-06-15",
  bio: "Veterano delle Enniolimpiadi. Specialista di ping pong e scacchi.",
  stats: {
    totalMatches: 47,
    wins: 32,
    losses: 15,
    winRate: 68.1,
    points: 2450,
    rank: 3,
    totalPlayers: 24
  },
  achievements: [
    { id: 1, name: "Campione Ping Pong", icon: "trophy", date: "2025-08-12", color: "amber" },
    { id: 2, name: "5 Vittorie Consecutive", icon: "flame", date: "2025-09-03", color: "orange" },
    { id: 3, name: "Partecipante Fondatore", icon: "star", date: "2024-06-15", color: "blue" },
  ],
  events: [
    { id: 1, name: "Ping Pong", format: "Eliminazione diretta", status: "completed", position: 1, wins: 4, losses: 0, points: 500 },
    { id: 2, name: "Scacchi", format: "Round Robin", status: "completed", position: 2, wins: 5, losses: 2, points: 400 },
    { id: 3, name: "Calcio Balilla", format: "Doppia eliminazione", status: "in_progress", position: null, wins: 3, losses: 1, points: 250 },
    { id: 4, name: "Freccette", format: "Gruppi + Eliminazione", status: "upcoming", position: null, wins: 0, losses: 0, points: 0 },
  ],
  recentMatches: [
    { id: 1, event: "Calcio Balilla", opponent: "Luca Bianchi", result: "win", score: "10-7", date: "2025-10-28" },
    { id: 2, event: "Calcio Balilla", opponent: "Giovanni Verdi", result: "win", score: "10-4", date: "2025-10-27" },
    { id: 3, event: "Scacchi", opponent: "Paolo Neri", result: "loss", score: "0-1", date: "2025-10-20" },
    { id: 4, event: "Calcio Balilla", opponent: "Andrea Marino", result: "loss", score: "8-10", date: "2025-10-15" },
    { id: 5, event: "Scacchi", opponent: "Luca Bianchi", result: "win", score: "1-0", date: "2025-10-12" },
  ],
  rivals: [
    { id: 2, name: "Luca Bianchi", matches: 8, wins: 5, losses: 3 },
    { id: 3, name: "Paolo Neri", matches: 6, wins: 3, losses: 3 },
    { id: 4, name: "Giovanni Verdi", matches: 5, wins: 4, losses: 1 },
  ]
};

function StatCard({ icon: Icon, label, value, subvalue, accent = "blue" }) {
  const accentColors = {
    blue: "text-blue-600 bg-blue-50",
    green: "text-green-600 bg-green-50",
    amber: "text-amber-600 bg-amber-50",
    purple: "text-purple-600 bg-purple-50"
  };
  
  return (
    <div className="bg-white rounded-xl p-4 shadow-sm border border-gray-100">
      <div className="flex items-center gap-3">
        <div className={`p-2 rounded-lg ${accentColors[accent]}`}>
          <Icon size={20} />
        </div>
        <div>
          <p className="text-2xl font-bold text-gray-900">{value}</p>
          <p className="text-sm text-gray-500">{label}</p>
          {subvalue && <p className="text-xs text-gray-400">{subvalue}</p>}
        </div>
      </div>
    </div>
  );
}

function AchievementBadge({ achievement }) {
  const icons = { trophy: Trophy, flame: Flame, star: Star, award: Award };
  const Icon = icons[achievement.icon] || Award;
  const colors = {
    amber: "bg-amber-100 text-amber-700 border-amber-200",
    orange: "bg-orange-100 text-orange-700 border-orange-200",
    blue: "bg-blue-100 text-blue-700 border-blue-200",
    green: "bg-green-100 text-green-700 border-green-200"
  };
  
  return (
    <div className={`inline-flex items-center gap-2 px-3 py-1.5 rounded-full border ${colors[achievement.color]}`}>
      <Icon size={14} />
      <span className="text-sm font-medium">{achievement.name}</span>
    </div>
  );
}

function EventRow({ event }) {
  const statusStyles = {
    completed: "bg-green-100 text-green-700",
    in_progress: "bg-blue-100 text-blue-700",
    upcoming: "bg-gray-100 text-gray-600"
  };
  const statusLabels = {
    completed: "Completato",
    in_progress: "In corso",
    upcoming: "In arrivo"
  };
  
  return (
    <div className="flex items-center justify-between py-3 border-b border-gray-100 last:border-0">
      <div className="flex-1">
        <div className="flex items-center gap-2">
          <h4 className="font-medium text-gray-900">{event.name}</h4>
          <span className={`text-xs px-2 py-0.5 rounded-full ${statusStyles[event.status]}`}>
            {statusLabels[event.status]}
          </span>
        </div>
        <p className="text-sm text-gray-500">{event.format}</p>
      </div>
      <div className="text-right">
        {event.position && (
          <p className="font-bold text-lg text-gray-900">
            {event.position === 1 ? "🥇" : event.position === 2 ? "🥈" : event.position === 3 ? "🥉" : `#${event.position}`}
          </p>
        )}
        <p className="text-sm text-gray-500">{event.wins}V - {event.losses}S</p>
        {event.points > 0 && <p className="text-xs text-blue-600">+{event.points} pts</p>}
      </div>
    </div>
  );
}

function MatchRow({ match }) {
  return (
    <div className="flex items-center gap-3 py-2.5 border-b border-gray-100 last:border-0">
      <div className={`w-1.5 h-8 rounded-full ${match.result === 'win' ? 'bg-green-500' : 'bg-red-400'}`} />
      <div className="flex-1 min-w-0">
        <p className="font-medium text-gray-900 truncate">{match.opponent}</p>
        <p className="text-xs text-gray-500">{match.event}</p>
      </div>
      <div className="text-right">
        <p className={`font-bold ${match.result === 'win' ? 'text-green-600' : 'text-red-500'}`}>
          {match.score}
        </p>
        <p className="text-xs text-gray-400">{new Date(match.date).toLocaleDateString('it-IT', { day: 'numeric', month: 'short' })}</p>
      </div>
    </div>
  );
}

function RivalCard({ rival }) {
  const winRate = Math.round((rival.wins / rival.matches) * 100);
  return (
    <div className="flex items-center gap-3 p-3 bg-gray-50 rounded-lg">
      <div className="w-10 h-10 rounded-full bg-gradient-to-br from-gray-300 to-gray-400 flex items-center justify-center text-white font-bold">
        {rival.name.charAt(0)}
      </div>
      <div className="flex-1">
        <p className="font-medium text-gray-900">{rival.name}</p>
        <p className="text-xs text-gray-500">{rival.matches} partite</p>
      </div>
      <div className="text-right">
        <p className="text-sm font-bold text-gray-900">{rival.wins}-{rival.losses}</p>
        <p className={`text-xs ${winRate >= 50 ? 'text-green-600' : 'text-red-500'}`}>{winRate}%</p>
      </div>
    </div>
  );
}

export default function PlayerProfile() {
  const [activeTab, setActiveTab] = useState('overview');
  const player = playerData;
  
  return (
    <div className="min-h-screen bg-gray-50">
      {/* Header */}
      <div className="bg-gradient-to-br from-blue-600 via-blue-700 to-indigo-800 text-white">
        <div className="max-w-4xl mx-auto px-4 pt-8 pb-20">
          <div className="flex items-start gap-4">
            <div className="w-20 h-20 rounded-2xl bg-white/20 backdrop-blur flex items-center justify-center text-3xl font-bold shadow-lg">
              {player.avatar ? (
                <img src={player.avatar} alt={player.name} className="w-full h-full rounded-2xl object-cover" />
              ) : (
                player.name.charAt(0)
              )}
            </div>
            <div className="flex-1">
              <h1 className="text-2xl font-bold">{player.name}</h1>
              {player.nickname && (
                <p className="text-blue-200 text-sm">"{player.nickname}"</p>
              )}
              <p className="text-blue-100 text-sm mt-1 opacity-80">{player.bio}</p>
            </div>
            <div className="text-right">
              <div className="text-3xl font-bold">#{player.stats.rank}</div>
              <div className="text-blue-200 text-sm">su {player.stats.totalPlayers}</div>
            </div>
          </div>
        </div>
      </div>
      
      {/* Stats Cards - overlapping header */}
      <div className="max-w-4xl mx-auto px-4 -mt-12">
        <div className="grid grid-cols-2 md:grid-cols-4 gap-3">
          <StatCard icon={Target} label="Partite" value={player.stats.totalMatches} accent="blue" />
          <StatCard icon={Trophy} label="Vittorie" value={player.stats.wins} subvalue={`${player.stats.winRate}%`} accent="green" />
          <StatCard icon={Medal} label="Punti" value={player.stats.points.toLocaleString()} accent="amber" />
          <StatCard icon={TrendingUp} label="Sconfitte" value={player.stats.losses} accent="purple" />
        </div>
      </div>
      
      {/* Tabs */}
      <div className="max-w-4xl mx-auto px-4 mt-6">
        <div className="flex gap-1 bg-gray-100 p-1 rounded-lg">
          {[
            { id: 'overview', label: 'Panoramica' },
            { id: 'events', label: 'Eventi' },
            { id: 'matches', label: 'Partite' },
          ].map(tab => (
            <button
              key={tab.id}
              onClick={() => setActiveTab(tab.id)}
              className={`flex-1 py-2 px-4 rounded-md text-sm font-medium transition-all ${
                activeTab === tab.id 
                  ? 'bg-white text-gray-900 shadow-sm' 
                  : 'text-gray-600 hover:text-gray-900'
              }`}
            >
              {tab.label}
            </button>
          ))}
        </div>
      </div>
      
      {/* Content */}
      <div className="max-w-4xl mx-auto px-4 py-6 space-y-6">
        {activeTab === 'overview' && (
          <>
            {/* Achievements */}
            <div className="bg-white rounded-xl p-5 shadow-sm border border-gray-100">
              <h3 className="font-semibold text-gray-900 mb-3 flex items-center gap-2">
                <Award size={18} className="text-amber-500" />
                Achievements
              </h3>
              <div className="flex flex-wrap gap-2">
                {player.achievements.map(a => (
                  <AchievementBadge key={a.id} achievement={a} />
                ))}
              </div>
            </div>
            
            {/* Recent + Rivals side by side on desktop */}
            <div className="grid md:grid-cols-2 gap-6">
              <div className="bg-white rounded-xl p-5 shadow-sm border border-gray-100">
                <h3 className="font-semibold text-gray-900 mb-3 flex items-center gap-2">
                  <Calendar size={18} className="text-blue-500" />
                  Partite Recenti
                </h3>
                <div>
                  {player.recentMatches.slice(0, 4).map(m => (
                    <MatchRow key={m.id} match={m} />
                  ))}
                </div>
              </div>
              
              <div className="bg-white rounded-xl p-5 shadow-sm border border-gray-100">
                <h3 className="font-semibold text-gray-900 mb-3 flex items-center gap-2">
                  <Users size={18} className="text-purple-500" />
                  Rivali
                </h3>
                <div className="space-y-2">
                  {player.rivals.map(r => (
                    <RivalCard key={r.id} rival={r} />
                  ))}
                </div>
              </div>
            </div>
          </>
        )}
        
        {activeTab === 'events' && (
          <div className="bg-white rounded-xl p-5 shadow-sm border border-gray-100">
            <h3 className="font-semibold text-gray-900 mb-4">Partecipazioni Eventi</h3>
            {player.events.map(e => (
              <EventRow key={e.id} event={e} />
            ))}
          </div>
        )}
        
        {activeTab === 'matches' && (
          <div className="bg-white rounded-xl p-5 shadow-sm border border-gray-100">
            <h3 className="font-semibold text-gray-900 mb-4">Storico Partite</h3>
            {player.recentMatches.map(m => (
              <MatchRow key={m.id} match={m} />
            ))}
            <button className="w-full mt-4 py-2 text-blue-600 text-sm font-medium hover:bg-blue-50 rounded-lg transition-colors flex items-center justify-center gap-1">
              Carica altre partite
              <ChevronRight size={16} />
            </button>
          </div>
        )}
      </div>
      
      {/* Footer */}
      <div className="max-w-4xl mx-auto px-4 pb-8">
        <p className="text-center text-xs text-gray-400">
          Membro dal {new Date(player.joinedDate).toLocaleDateString('it-IT', { month: 'long', year: 'numeric' })}
        </p>
      </div>
    </div>
  );
}
